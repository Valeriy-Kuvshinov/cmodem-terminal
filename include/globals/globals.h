#ifndef GLOBALS_H
#define GLOBALS_H

#include <signal.h>

/* Global exit flag for immediate thread termination */
extern volatile sig_atomic_t exit_requested;

#endif