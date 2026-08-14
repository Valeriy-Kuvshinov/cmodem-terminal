#ifndef THREADS_H
#define THREADS_H

#include <poll.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "../globals/globals.h"
#include "../io/serial.h"
#include "../modem/modem.h"
#include "../modem/parser.h"
#include "../utils/utils.h"

/* SMS mode handling */
#define SMS_MODE_ON true
#define SMS_MODE_OFF false
#define EXIT_SIGNAL -1

#define EXIT_APP_COMMAND "exit"

/* Function prototypes */
/* Thread lifecycle */
bool init_terminal(const char *device_port);

void start_threads(pthread_t *modem_thread, pthread_t *stdin_thread);

void exit_threads(pthread_t modem_thread, pthread_t stdin_thread);

void cleanup_terminal();

/* Thread state */
void set_terminal_running(bool value);

/* Thread functions */
void *read_modem_thread(void *arg);

void *read_stdin_thread(void *arg);

#endif