#ifndef MODEM_H
#define MODEM_H

#include <pthread.h>
#include <stdbool.h>

#include "../globals/buffers.h"

typedef struct {
  int fd;
  bool is_running;
  char output_buffer[MAX_BUFFER];
  size_t buffer_length;
  char last_command[MAX_COMMAND];
  pthread_mutex_t serial_mutex;
  pthread_mutex_t running_mutex;
} ModemTerminal;

/* Retry configuration */
#define MAX_INIT_RETRIES 3

/* Function prototypes */
int check_urgent_message(const char *buffer);
void categorize_and_output_line(const ModemTerminal *term, const char *line);
int init_modem(ModemTerminal *term);

#endif