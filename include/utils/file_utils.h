#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

#include "../globals/chars.h"

/* Function prototypes */
void safe_fclose(FILE **fp);
FILE *open_source_file(const char *filename);

#endif