#ifndef THREADS_H
#define THREADS_H

#include "modem.h"

// Function prototypes for threads
void set_running(ModemTerminal *term, int value);
void *read_modem_thread(void *arg);
void *read_stdin_thread(void *arg);

#endif