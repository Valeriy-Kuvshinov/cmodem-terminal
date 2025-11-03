#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

/* Global flag for thread identification while logging */
extern _Thread_local const char *thread_name;

/* Global exit flag for immediate thread termination */
extern _Alignas(32) _Atomic bool exit_requested;

/* App launch flags */
#define QUIET_MODE_FLAG "--quiet"

#define HAS_QUIET_MODE_FLAG(argc, argv)                                        \
  (strcmp((argv)[2], QUIET_MODE_FLAG) == 0)

#endif