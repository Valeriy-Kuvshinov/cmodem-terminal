#ifndef THREADS_H
#define THREADS_H

#include "../globals/globals.h"
#include "../io/serial.h"
#include "../modem/modem.h"
#include "../modem/parser.h"
#include "../utils/utils.h"

/* SMS mode handling */
#define SMS_MODE_ON true
#define SMS_MODE_OFF false
#define EXIT_SIGNAL -1

#define MAX_PORT_RETRIES 3

#define EXIT_APP_COMMAND "exit"

/* Macro for handling quit terminal */
#define IS_EXIT_COMMAND(line) (strcasecmp(line, EXIT_APP_COMMAND) == 0)

/* Macros for SMS handling */
#define IS_SMS_COMMAND(line)                                                   \
  (strncmp(line, AT_SEND_SMS "=", AT_SEND_SMS_LENGTH + 1) == 0)

/* Macros for input validation */
#define HAS_NEWLINE(str) (strchr((str), NEWLINE) != NULL)
#define IS_BUFFER_FULL(str, size) (strlen(str) == (size) - 1)

/* Macros for character reading */
#define READ_UNTIL_NEWLINE_OR_EOF(c) ((c = getchar()) != NEWLINE && c != EOF)

/* Function prototypes */
/* Thread lifecycle */
int init_terminal(const char *device_port);
void start_threads(pthread_t *modem_thread, pthread_t *stdin_thread);
void exit_threads(pthread_t modem_thread, pthread_t stdin_thread);
void cleanup_terminal();

/* Thread state */
int is_terminal_running(void);
void set_terminal_running(bool value);

/* Thread functions */
void *read_modem_thread(void *arg);
void *read_stdin_thread(void *arg);

#endif