#include <string.h>

#include "../include/core/modem.h"
#include "../include/threads/threads.h"
#include "../include/utils/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static int is_args_valid(int argc, char *argv[], int *quiet_mode) {
  if (argc < 2 || argc > 4) {
    fprintf(stderr, "Usage: %s <device_port> [--quiet]%c", argv[0], NEWLINE);

    return 0;
  }

  *quiet_mode = HAS_QUIET_MODE_FLAG(argc, argv);

  if (argc == 3 && !HAS_QUIET_MODE_FLAG(argc, argv)) {
    fprintf(stderr, "Usage: %s <device_port> [--quiet]%c", argv[0], NEWLINE);

    return 0;
  }
  return 1;
}

static int is_connection_stable(int fd) {
  char response[MAX_RESPONSE];
  int n;

  print_output(MSG_TYPE_STATUS, "Testing modem connection...");

  safe_write(fd, AT_CRLF, AT_CRLF_LENGTH);
  sleep(1);

  n = read(fd, response, sizeof(response) - 1);

  if (n > 0) {
    response[n] = NULL_TERMINATOR;

    if (strstr(response, MODEM_AT_RESPONSE_OK))
      return 1;
  }
  print_output(MSG_TYPE_ERROR, "No response to initial AT command");

  return 0;
}

/* App main method */
/* ==================================================================== */
int main(int argc, char *argv[]) {
  const char *device_port;
  int quiet_mode;
  pthread_t modem_thread, stdin_thread;
  ModemTerminal term;

  if (!is_args_valid(argc, argv, &quiet_mode))
    return 1;

  device_port = argv[1];

  if (!init_terminal(&term, device_port))
    return 1;

  if (!is_connection_stable(term.fd)) {
    close(term.fd);

    return 1;
  }

  if (!init_modem(&term)) {
    close(term.fd);

    return 1;
  }

  print_output(MSG_TYPE_STATUS, "INIT COMPLETE");

  if (!quiet_mode) {
    printf("You may start writing AT commands.%c", NEWLINE);
    printf("Type '%s' to quit terminal.%c", EXIT_APP_COMMAND, NEWLINE);
  }

  start_threads(&term, &modem_thread, &stdin_thread);

  wait_for_threads(&term, modem_thread, stdin_thread);

  cleanup_resources(&term);

  return 0;
}
