#include <string.h>

#include "../../include/globals/globals.h"
#include "../../include/io/serial.h"
#include "../../include/threads/threads.h"

/* Outer methods */
/* ==================================================================== */
int is_running(ModemTerminal *term) {
  int running;

  pthread_mutex_lock(&term->running_mutex);

  running = term->running;

  pthread_mutex_unlock(&term->running_mutex);

  return running;
}

void set_running(ModemTerminal *term, int value) {
  pthread_mutex_lock(&term->running_mutex);

  term->running = value;

  pthread_mutex_unlock(&term->running_mutex);
}

int init_terminal(ModemTerminal *term, const char *device_port) {
  memset(term, 0, sizeof(ModemTerminal));

  term->running = TRUE;

  pthread_mutex_init(&term->serial_mutex, NULL);
  pthread_mutex_init(&term->running_mutex, NULL);

  term->fd =
      open_serial_port(device_port, MAX_PORT_RETRIES, PORT_RETRY_DELAY_SEC);

  if (term->fd < 0) {
    pthread_mutex_destroy(&term->serial_mutex);
    pthread_mutex_destroy(&term->running_mutex);

    return FALSE;
  }
  return TRUE;
}

void start_threads(ModemTerminal *term, pthread_t *modem_thread,
                   pthread_t *stdin_thread) {
  pthread_create(modem_thread, NULL, read_modem_thread, term);
  pthread_create(stdin_thread, NULL, read_stdin_thread, term);
}

void wait_for_threads(ModemTerminal *term, pthread_t modem_thread,
                      pthread_t stdin_thread) {
  if (exit_requested) {
    // Immediate exit: cancel threads instead of waiting for sleep cycles
    pthread_cancel(modem_thread);
    pthread_cancel(stdin_thread);

    // Wait briefly for cancellation to complete
    pthread_join(stdin_thread, NULL);
    pthread_join(modem_thread, NULL);
  } else {
    // Normal exit: wait for threads to finish their current operations
    pthread_join(stdin_thread, NULL);

    set_running(term, FALSE);

    pthread_join(modem_thread, NULL);
  }
}

void cleanup_resources(ModemTerminal *term) {
  close(term->fd);

  pthread_mutex_destroy(&term->serial_mutex);
  pthread_mutex_destroy(&term->running_mutex);

  print_output(MSG_TYPE_STATUS, "Modem terminal stopped");
}