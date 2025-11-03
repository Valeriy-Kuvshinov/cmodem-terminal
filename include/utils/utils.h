#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/* Function prototypes */
ssize_t safe_write(int fd, const void *buf, size_t count);

void print_output(const char *type, const char *text);
void msleep(int milliseconds);

#endif