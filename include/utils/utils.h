#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include "../globals/chars.h"
#include "../globals/globals.h"
#include "../globals/time.h"

/* Function prototypes */
ssize_t safe_write(int fd, const void *buf, size_t count);

void print_output(const char *type, const char *text);
void msleep(int millis);
bool is_whitespace_only(const char *str);
char *trim_whitespace(char *str);

/* Macros */
#define IS_REAL_ERROR(bytes_read) ((bytes_read) < 0 && errno != EAGAIN && errno != EWOULDBLOCK)

#endif