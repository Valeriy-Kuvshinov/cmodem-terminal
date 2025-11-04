#ifndef PARSER_H
#define PARSER_H

#include "commands.h"
#include "modem.h"
#include "responses.h"

/* Function prototypes */
void categorize_line(const ModemTerminal *term, const char *line);

#endif