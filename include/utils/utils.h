#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>

#include "../globals/chars.h"
#include "../globals/globals.h"
#include "../globals/time.h"

/* Function prototypes */
ssize_t safe_write(int fd, const void *buf, size_t count);

void print_output(const char *type, const char *text);
void msleep(int milliseconds);
bool is_whitespace_only(const char *str);

#endif