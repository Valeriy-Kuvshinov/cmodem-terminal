#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>

#define MIN_APP_ARGUMENTS 2
#define MAX_APP_ARGUMENTS 4

/* App launch flags */
#define SERVER_MODE_FLAG "--server"
#define CONFIG_MODE_FLAG "--config"

/* Global app state */
extern bool server_mode;
extern bool config_mode;

#endif