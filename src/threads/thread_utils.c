#include "../../include/threads/threads.h"

/* Inner STATIC methods */
/* ==================================================================== */
/* Immediate exit: cancel threads instead of waiting for sleep cycles */
void fast_exit_threads(pthread_t modem_thread, pthread_t stdin_thread) {
  pthread_cancel(modem_thread);
  pthread_cancel(stdin_thread);

  /* Wait briefly for cancellation to complete */
  pthread_join(stdin_thread, NULL);
  pthread_join(modem_thread, NULL);
}

/* Normal exit: wait for threads to finish their current operations */
void normal_exit_threads(pthread_t modem_thread, pthread_t stdin_thread) {
  pthread_join(stdin_thread, NULL);

  set_terminal_running(false);

  pthread_join(modem_thread, NULL);
}

/* Outer methods */
/* ==================================================================== */
int is_terminal_running(void) {
  int running;

  pthread_mutex_lock(&terminal.running_mutex);

  running = terminal.is_running;

  pthread_mutex_unlock(&terminal.running_mutex);

  return running;
}

void set_terminal_running(bool value) {
  pthread_mutex_lock(&terminal.running_mutex);

  terminal.is_running = value;

  pthread_mutex_unlock(&terminal.running_mutex);
}

void start_threads(pthread_t *modem_thread, pthread_t *stdin_thread) {
  pthread_create(modem_thread, NULL, read_modem_thread, NULL);
  pthread_create(stdin_thread, NULL, read_stdin_thread, NULL);
}

void wait_for_threads(pthread_t modem_thread, pthread_t stdin_thread) {
  if (atomic_load(&exit_requested))
    fast_exit_threads(modem_thread, stdin_thread);
  else
    normal_exit_threads(modem_thread, stdin_thread);
}

void cleanup_resources(void) {
  close(terminal.fd);

  pthread_mutex_destroy(&terminal.serial_mutex);
  pthread_mutex_destroy(&terminal.running_mutex);

  print_output(MSG_TYPE_STATUS, "Modem terminal stopped");
}