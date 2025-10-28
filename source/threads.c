#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "../headers/threads.h"
#include "../headers/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static int is_running(ModemTerminal *term) {
  int running;

  pthread_mutex_lock(&term->running_mutex);

  running = term->running;

  pthread_mutex_unlock(&term->running_mutex);

  return running;
}

static void send_sms_command(ModemTerminal *term, const char *line) {
  pthread_mutex_lock(&term->serial_mutex);
  strncpy(term->last_command, AT_SEND_SMS, sizeof(term->last_command));

  safe_write(term->fd, line, strlen(line));
  safe_write(term->fd, CRLF, CRLF_LENGTH);

  pthread_mutex_unlock(&term->serial_mutex);
  msleep(SMS_SEND_DELAY_MS);
}

static int is_sms_end_marker(const char *line) {
  return line[0] == SMS_END_MARKER && line[1] == NULL_TERMINATOR;
}

static void send_sms_end_marker(ModemTerminal *term) {
  pthread_mutex_lock(&term->serial_mutex);

  char sms_end_marker = CTRL_Z;

  safe_write(term->fd, &sms_end_marker, 1);

  pthread_mutex_unlock(&term->serial_mutex);
  msleep(COMMAND_DELAY_MS);
}

static void send_sms_content(ModemTerminal *term, const char *line) {
  pthread_mutex_lock(&term->serial_mutex);

  safe_write(term->fd, line, strlen(line));

  pthread_mutex_unlock(&term->serial_mutex);
}

static void send_regular_command(ModemTerminal *term, const char *line) {
  pthread_mutex_lock(&term->serial_mutex);
  strncpy(term->last_command, line, sizeof(term->last_command));

  safe_write(term->fd, line, strlen(line));
  safe_write(term->fd, CRLF, CRLF_LENGTH);

  pthread_mutex_unlock(&term->serial_mutex);
  msleep(COMMAND_DELAY_MS);
}

static int process_stdin_line(ModemTerminal *term, char *line, int sms_mode) {
  // Handle exit
  if (strcasecmp(line, EXIT_APP_COMMAND) == 0) {
    set_running(term, 0);

    return -1; // Exit signal
  }

  // Handle SMS mode
  if (strncmp(line, AT_SEND_SMS "=", AT_SEND_SMS_LENGTH + 1) == 0) {
    send_sms_command(term, line);

    return 1; // Enter SMS mode
  } else if (sms_mode && is_sms_end_marker(line)) {
    send_sms_end_marker(term);

    return 0; // Exit SMS mode
  } else if (sms_mode) {
    send_sms_content(term, line);

    return 1; // Stay in SMS mode
  } else {
    send_regular_command(term, line);

    return 0; // Regular mode
  }
}

static void handle_remaining_buffer(ModemTerminal *term, char *line_start) {
  // Check for urgent messages in remaining buffer
  if (check_urgent_message(line_start)) {
    print_output(MSG_TYPE_URGENT, line_start);

    line_start += strlen(line_start);
  }
  // Move remaining data to start of buffer
  term->buffer_len = strlen(line_start);

  if (term->buffer_len > 0 && line_start != term->output_buffer)
    memmove(term->output_buffer, line_start, term->buffer_len + 1);
  else if (term->buffer_len == 0)
    term->output_buffer[0] = NULL_TERMINATOR;
}

static void add_to_buffer(ModemTerminal *term, const char *data, int len) {
  if (term->buffer_len + len < MAX_BUFFER - 1) {
    memcpy(term->output_buffer + term->buffer_len, data, len);

    term->buffer_len += len;
    term->output_buffer[term->buffer_len] = NULL_TERMINATOR;
  }
}

static void process_complete_lines(ModemTerminal *term) {
  char *line_start = term->output_buffer;
  char *crlf;

  while ((crlf = strstr(line_start, CRLF)) != NULL) {
    *crlf = NULL_TERMINATOR;

    if (strlen(line_start) > 0)
      categorize_and_output_line(term, line_start);

    line_start = crlf + CRLF_LENGTH;
  }
  handle_remaining_buffer(term, line_start);
}

static void process_received_data(ModemTerminal *term, char *temp_buf, int n) {
  temp_buf[n] = NULL_TERMINATOR;

  pthread_mutex_lock(&term->serial_mutex);
  add_to_buffer(term, temp_buf, n);

  process_complete_lines(term);
  pthread_mutex_unlock(&term->serial_mutex);
}

/* Outer methods */
/* ==================================================================== */
void set_running(ModemTerminal *term, int value) {
  pthread_mutex_lock(&term->running_mutex);

  term->running = value;

  pthread_mutex_unlock(&term->running_mutex);
}

void *read_modem_thread(void *arg) {
  ModemTerminal *term = (ModemTerminal *)arg;

  char temp_buf[MAX_BUFFER];

  while (is_running(term)) {
    int n = read(term->fd, temp_buf, sizeof(temp_buf) - 1);

    if (n > 0)
      process_received_data(term, temp_buf, n);

    msleep(THREAD_SLEEP_MS);
  }
  return NULL;
}

void *read_stdin_thread(void *arg) {
  ModemTerminal *term = (ModemTerminal *)arg;

  char line[MAX_COMMAND];
  int sms_mode = 0;

  while (is_running(term)) {
    if (fgets(line, sizeof(line), stdin) != NULL) {
      // Remove newline
      size_t len = strlen(line);

      if (len > 0 && line[len - 1] == NEWLINE)
        line[len - 1] = NULL_TERMINATOR;

      if (strlen(line) > 0) {
        int new_sms_mode = process_stdin_line(term, line, sms_mode);

        if (new_sms_mode == -1)
          break; // Exit signal

        sms_mode = new_sms_mode;
      }
    }
    if (!is_running(term))
      break;
  }
  return NULL;
}
