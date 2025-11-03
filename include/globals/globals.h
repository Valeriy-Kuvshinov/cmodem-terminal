#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdatomic.h>
#include <stdbool.h>

/* Global exit flag for immediate thread termination */
extern _Atomic bool exit_requested;

/* App launch flags */
#define QUIET_MODE_FLAG "--quiet"

#define HAS_QUIET_MODE_FLAG(argc, argv)                                        \
  (strcmp((argv)[2], QUIET_MODE_FLAG) == 0)

#endif