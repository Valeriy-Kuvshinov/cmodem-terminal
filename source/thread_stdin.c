#include <string.h>

#include "../headers/threads.h"
#include "../headers/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void send_sms_command(ModemTerminal *term, const char *line) {
  pthread_mutex_lock(&term->serial_mutex);
  strncpy(term->last_command, AT_SEND_SMS, sizeof(term->last_command));

  safe_write(term->fd, line, strlen(line));
  safe_write(term->fd, CRLF, CRLF_LENGTH);

  pthread_mutex_unlock(&term->serial_mutex);
  msleep(SMS_SEND_DELAY_MS);
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
  if (IS_EXIT_COMMAND(line)) {
    set_running(term, 0);

    return -1; // Exit signal
  }

  // Handle SMS mode
  if (IS_SMS_COMMAND(line)) {
    send_sms_command(term, line);

    return SMS_MODE_ON; // Enter SMS mode
  } else if (sms_mode && IS_SMS_END_MARKER(line)) {
    send_sms_end_marker(term);

    return SMS_MODE_OFF; // Exit SMS mode
  } else if (sms_mode) {
    send_sms_content(term, line);

    return SMS_MODE_ON; // Stay in SMS mode
  } else {
    send_regular_command(term, line);

    return SMS_MODE_OFF; // Regular mode
  }
}

/* Outer methods */
/* ==================================================================== */
void *read_stdin_thread(void *arg) {
  char line[MAX_COMMAND];
  int sms_mode = 0;
  ModemTerminal *term = (ModemTerminal *)arg;

  while (is_running(term)) {
    if (fgets(line, sizeof(line), stdin) != NULL) {
      size_t len = strlen(line);

      if (len > 0 && line[len - 1] == NEWLINE)
        line[len - 1] = NULL_TERMINATOR;

      if (strlen(line) > 0) {
        int new_mode = process_stdin_line(term, line, sms_mode);

        if (new_mode == -1)
          break;

        sms_mode = new_mode;
      }
    }
    if (!is_running(term))
      break;
  }
  return NULL;
}
