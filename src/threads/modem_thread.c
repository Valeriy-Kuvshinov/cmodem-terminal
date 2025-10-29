#include <string.h>

#include "../../include/threads/threads.h"
#include "../../include/utils/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void handle_remaining_buffer(ModemTerminal *term, char *line_start) {
  if (check_urgent_message(line_start)) {
    print_output(MSG_TYPE_URGENT, line_start);

    line_start += strlen(line_start);
  }
  term->buffer_length = strlen(line_start);

  if (term->buffer_length > 0 && line_start != term->output_buffer)
    memmove(term->output_buffer, line_start, term->buffer_length + 1);

  else if (term->buffer_length == 0)
    term->output_buffer[0] = NULL_TERMINATOR;
}

static void add_to_buffer(ModemTerminal *term, const char *data, int length) {
  if (term->buffer_length + length < MAX_BUFFER - 1) {
    memcpy(term->output_buffer + term->buffer_length, data, length);

    term->buffer_length += length;
    term->output_buffer[term->buffer_length] = NULL_TERMINATOR;
  }
}

static void process_complete_lines(ModemTerminal *term) {
  char *crlf;
  char *line_start = term->output_buffer;

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
void *read_modem_thread(void *arg) {
  char temp_buf[MAX_BUFFER];
  ModemTerminal *term = (ModemTerminal *)arg;

  while (is_running(term)) {
    int n = read(term->fd, temp_buf, sizeof(temp_buf) - 1);

    if (n > 0)
      process_received_data(term, temp_buf, n);

    msleep(THREAD_SLEEP_MS);
  }
  return NULL;
}