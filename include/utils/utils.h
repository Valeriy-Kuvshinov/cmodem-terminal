#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "../globals/chars.h"
#include "../globals/globals.h"
#include "../globals/time.h"

/* Function prototypes */
ssize_t safe_write(int fd, const void *buf, size_t count);

void print_output(const char *type, const char *text);

void msleep(int millis);

bool is_whitespace_only(const char *str);

char *trim_whitespace(char *str);

#endif