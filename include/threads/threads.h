#ifndef THREADS_H
#define THREADS_H

#include <strings.h>

#include "../core/modem.h"
#include "../utils/utils.h"

#define SMS_MODE_ON 1
#define SMS_MODE_OFF 0

/* Retry configuration */
#define MAX_PORT_RETRIES 3
#define PORT_RETRY_DELAY_SEC 8

/* Timing constants (milliseconds) */
#define COMMAND_DELAY_MS 100
#define SMS_SEND_DELAY_MS 500
#define THREAD_SLEEP_MS 10

/* Macros */
#define IS_EXIT_COMMAND(line) (strcasecmp(line, EXIT_APP_COMMAND) == 0)

/* Macros for SMS handling */
#define IS_SMS_COMMAND(line)                                                   \
  (strncmp(line, AT_SEND_SMS "=", AT_SEND_SMS_LENGTH + 1) == 0)
#define IS_SMS_END_MARKER(line)                                                \
  (line[0] == SMS_END_MARKER && line[1] == NULL_TERMINATOR)

// Thread lifecycle
int init_terminal(ModemTerminal *term, const char *device_port);
void start_threads(ModemTerminal *term, pthread_t *modem_thread,
                   pthread_t *stdin_thread);
void wait_for_threads(ModemTerminal *term, pthread_t modem_thread,
                      pthread_t stdin_thread);
void cleanup_resources(ModemTerminal *term);

// Thread state
int is_running(ModemTerminal *term);
void set_running(ModemTerminal *term, int value);

// Thread functions
void *read_modem_thread(void *arg);
void *read_stdin_thread(void *arg);

#endif