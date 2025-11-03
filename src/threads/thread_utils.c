
#include "../../include/globals/globals.h"
#include "../../include/modem/responses.h"
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
void normal_exit_threads(ModemTerminal *term, pthread_t modem_thread,
                         pthread_t stdin_thread) {
  pthread_join(stdin_thread, NULL);

  set_terminal_running(term, false);

  pthread_join(modem_thread, NULL);
}

/* Outer methods */
/* ==================================================================== */
int is_terminal_running(ModemTerminal *term) {
  int running;

  pthread_mutex_lock(&term->running_mutex);

  running = term->is_running;

  pthread_mutex_unlock(&term->running_mutex);

  return running;
}

void set_terminal_running(ModemTerminal *term, bool value) {
  pthread_mutex_lock(&term->running_mutex);

  term->is_running = value;

  pthread_mutex_unlock(&term->running_mutex);
}

void start_threads(ModemTerminal *term, pthread_t *modem_thread,
                   pthread_t *stdin_thread) {
  pthread_create(modem_thread, NULL, read_modem_thread, term);
  pthread_create(stdin_thread, NULL, read_stdin_thread, term);
}

void wait_for_threads(ModemTerminal *term, pthread_t modem_thread,
                      pthread_t stdin_thread) {
  if (atomic_load(&exit_requested))
    fast_exit_threads(modem_thread, stdin_thread);
  else
    normal_exit_threads(term, modem_thread, stdin_thread);
}

void cleanup_resources(ModemTerminal *term) {
  close(term->fd);

  pthread_mutex_destroy(&term->serial_mutex);
  pthread_mutex_destroy(&term->running_mutex);

  print_output(MSG_TYPE_STATUS, "Modem terminal stopped");
}