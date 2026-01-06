#include "../../include/globals/globals.h"
#include "../../include/config.h"
#include "../../include/modem/call.h"
#include "../../include/modem/modem.h"

/* Global application state */
ModemTerminal terminal;
CallState call_state = {0};
Config config;
