#ifndef MODEM_H
#define MODEM_H

#include <pthread.h>
#include <stdbool.h>

#include "../globals/buffers.h"
#include "../globals/chars.h"
#include "../globals/time.h"
#include "../modem/commands.h"
#include "../modem/responses.h"
#include "../utils/utils.h"

/* Retry configuration */
#define MAX_INIT_RETRIES 3

typedef struct {
  int fd;
  bool is_running;
  char output_buffer[MAX_BUFFER];
  size_t buffer_length;
  char last_command[MAX_COMMAND];
  pthread_mutex_t serial_mutex;
  pthread_mutex_t running_mutex;
} ModemTerminal;

extern ModemTerminal terminal;

/* Function prototypes */
int init_modem(void);

#endif