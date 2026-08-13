#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>

/* App launch flags */
#define QUIET_MODE_FLAG "--quiet"
#define TIMELESS_MODE_FLAG "--timeless"

/* Global app state */
extern bool timeless_mode;

#endif