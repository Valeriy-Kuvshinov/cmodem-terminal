#include <signal.h>

#include "../include/modem/call.h"
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

static void signal_handler(int signum) {
  print_output(MSG_TYPE_STATUS, "Shutting down...");
  set_terminal_running(false);
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

    if (strstr(response, MODEM_RESPONSE_OK))
      return 1;
  }
  /* Register signal handlers for graceful shutdown */
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  print_output(MSG_TYPE_ERROR, "No response to initial AT command");

  return 0;
}

/* App main method */
/* ==================================================================== */
int main(int argc, char *argv[]) {
  const char *device_port;
  int quiet_mode;
  pthread_t modem_thread, stdin_thread;

  if (!is_args_valid(argc, argv, &quiet_mode))
    return 1;

  device_port = argv[1];

  if (!init_terminal(device_port))
    return 1;

  if (!is_connection_stable(terminal.fd) || !init_modem()) {
    close(terminal.fd);

    return 1;
  }

  init_call_state();

  print_output(MSG_TYPE_STATUS, "INIT COMPLETE");

  if (!quiet_mode) {
    printf("You may start writing AT commands.%c", NEWLINE);
    printf("Type '%s' to quit terminal.%c", EXIT_APP_COMMAND, NEWLINE);
  }

  start_threads(&modem_thread, &stdin_thread);

  exit_threads(modem_thread, stdin_thread);

  cleanup();

  return 0;
}
