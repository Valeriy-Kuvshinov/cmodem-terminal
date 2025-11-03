#include "../../include/threads/threads.h"

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

static void complete_sms_sending(ModemTerminal *term) {
  char sms_end_marker = CTRL_Z;

  pthread_mutex_lock(&term->serial_mutex);

  safe_write(term->fd, &sms_end_marker, 1);

  pthread_mutex_unlock(&term->serial_mutex);

  msleep(COMMAND_DELAY_MS);
}

static void send_sms_content(ModemTerminal *term, const char *line) {
  pthread_mutex_lock(&term->serial_mutex);

  safe_write(term->fd, line, strlen(line));

  pthread_mutex_unlock(&term->serial_mutex);
}

static void send_command(ModemTerminal *term, const char *line) {
  pthread_mutex_lock(&term->serial_mutex);

  strncpy(term->last_command, line, sizeof(term->last_command));

  safe_write(term->fd, line, strlen(line));
  safe_write(term->fd, CRLF, CRLF_LENGTH);

  pthread_mutex_unlock(&term->serial_mutex);

  msleep(COMMAND_DELAY_MS);
}

static int process_stdin_line(ModemTerminal *term, char *line, int sms_mode) {
  if (IS_EXIT_COMMAND(line)) {
    atomic_store(&exit_requested, true);

    set_terminal_running(term, false);

    return EXIT_SIGNAL;
  }
  if (IS_SMS_COMMAND(line)) {
    send_sms_command(term, line);

    return SMS_MODE_ON;
  } else if (sms_mode) {
    send_sms_content(term, line);
    complete_sms_sending(term);

    return SMS_MODE_OFF;
  } else {
    send_command(term, line);

    return SMS_MODE_OFF;
  }
}

static int sanitize_input(char *line, size_t buffer_size) {
  line[buffer_size - 1] = NULL_TERMINATOR;

  if (!HAS_NEWLINE(line) && IS_BUFFER_FULL(line, buffer_size))
    return 0;

  size_t length = strlen(line);

  if (length > 0 && line[length - 1] == NEWLINE)
    line[length - 1] = NULL_TERMINATOR;

  return 1;
}

static void clear_stdin_buffer(void) {
  int c;

  while (READ_UNTIL_NEWLINE_OR_EOF(c))
    ;
}

/* Outer methods */
/* ==================================================================== */
void *read_stdin_thread(void *arg) {
  char line[MAX_COMMAND];
  int sms_mode = false;
  thread_name = "INPUT";
  ModemTerminal *term = (ModemTerminal *)arg;

  while (is_terminal_running(term)) {
    if (fgets(line, sizeof(line), stdin) != NULL) {
      if (!sanitize_input(line, sizeof(line))) {
        clear_stdin_buffer();

        print_output(MSG_TYPE_WARNING, "Input too long - ignored");

        continue;
      }
      if (strlen(line) > 0) {
        int new_mode = process_stdin_line(term, line, sms_mode);

        if (new_mode == EXIT_SIGNAL)
          break;

        sms_mode = new_mode;
      }
    }
    if (!is_terminal_running(term))
      break;
  }
  return NULL;
}
