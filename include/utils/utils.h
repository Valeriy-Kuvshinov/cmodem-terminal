#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include "../globals/chars.h"
#include "../globals/globals.h"
#include "../globals/time.h"
#include "../modem/responses.h"

/* ANSI escape codes for terminal manipulation */
#define ANSI_CURSOR_UP "\033[1A"
#define ANSI_ERASE_LINE "\033[2K"

/* General Function prototypes */
ssize_t safe_write(int fd, const void *buf, size_t count);

void print_output(const char *type, const char *text);

void msleep(int millis);

bool is_whitespace_only(const char *str);

char *trim_whitespace(char *str);

/* File Function prototypes */
void safe_fclose(FILE **fp);

FILE *open_source_file(const char *filename);

#endif