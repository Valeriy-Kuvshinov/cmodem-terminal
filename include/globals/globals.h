#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>

/* App launch flags */
#define QUIET_MODE_FLAG "--quiet"

#define HAS_QUIET_MODE_FLAG(argc, argv)                                        \
	(strcmp((argv)[2], QUIET_MODE_FLAG) == 0)

#endif