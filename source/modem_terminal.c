#include <string.h>

#include "../headers/modem.h"
#include "../headers/serial.h"
#include "../headers/threads.h"
#include "../headers/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static int is_args_valid(int argc, char *argv[]) {
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s <device_port>%c", argv[0], NEWLINE);

    return 0;
  }
  return 1;
}

static void init_modem_terminal(ModemTerminal *term, const char *device_port) {
  memset(term, 0, sizeof(ModemTerminal));
  term->running = 1;

  pthread_mutex_init(&term->serial_mutex, NULL);
  pthread_mutex_init(&term->running_mutex, NULL);

  // Open serial port
  term->fd =
      open_serial_port(device_port, MAX_PORT_RETRIES, PORT_RETRY_DELAY_SEC);
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
      return 1; // Success
  }
  print_output(MSG_TYPE_ERROR, "No response to initial AT command");

  return 0; // Failure
}

static void start_threads(ModemTerminal *term, pthread_t *modem_thread,
                          pthread_t *stdin_thread) {
  pthread_create(modem_thread, NULL, read_modem_thread, term);
  pthread_create(stdin_thread, NULL, read_stdin_thread, term);
}

static void wait_for_threads(ModemTerminal *term, pthread_t modem_thread,
                             pthread_t stdin_thread) {
  // Wait for exit
  pthread_join(stdin_thread, NULL);
  set_running(term, 0);
  pthread_join(modem_thread, NULL);
}

static void cleanup_resources(ModemTerminal *term) {
  close(term->fd);

  pthread_mutex_destroy(&term->serial_mutex);
  pthread_mutex_destroy(&term->running_mutex);

  print_output(MSG_TYPE_STATUS, "Modem terminal stopped");
}

int main(int argc, char *argv[]) {
  ModemTerminal term;
  pthread_t modem_thread, stdin_thread;
  const char *device_port;

  if (!is_args_valid(argc, argv))
    return 1;

  device_port = argv[1];

  init_modem_terminal(&term, device_port);

  if (term.fd < 0)
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

  printf("You may start writing AT commands.%c", NEWLINE);
  printf("Type %s to quit terminal.%c", EXIT_APP_COMMAND, NEWLINE);

  start_threads(&term, &modem_thread, &stdin_thread);

  wait_for_threads(&term, modem_thread, stdin_thread);

  cleanup_resources(&term);

  return 0;
}
