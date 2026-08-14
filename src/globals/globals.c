#include "../../include/globals/globals.h"
#include "../../include/config.h"
#include "../../include/modem/call.h"
#include "../../include/modem/modem.h"

/* Global application state */
Config config;
ModemTerminal terminal;
CallState call_state = {0};

/* Application configuration flags */
bool timeless_mode = false;
bool remote_mode = false;
