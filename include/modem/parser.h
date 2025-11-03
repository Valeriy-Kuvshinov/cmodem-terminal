#ifndef PARSER_H
#define PARSER_H

#include "commands.h"
#include "modem.h"
#include "responses.h"

int check_urgent_message(const char *buffer);
void categorize_line(const ModemTerminal *term, const char *line);

#endif