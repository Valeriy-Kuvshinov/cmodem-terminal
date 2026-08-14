#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>

#define MIN_APP_ARGUMENTS 2
#define MAX_APP_ARGUMENTS 5

/* App launch flags */
#define QUIET_MODE_FLAG "--quiet"
#define TIMELESS_MODE_FLAG "--timeless"
#define REMOTE_MODE_FLAG "--remote"

/* Global app state */
extern bool timeless_mode;
extern bool remote_mode;

#endif