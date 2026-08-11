#include "../../include/utils/file_utils.h"

void safe_fclose(FILE **fp) {
	if (fp && *fp) {
		fclose(*fp);
		*fp = NULL;
	}
}

FILE *open_source_file(const char *filename) {
	FILE *fp = fopen(filename, "r");

	if (!fp) {
		printf("Error: Failed to open file (%s)%c", filename, NEWLINE);
	}
	return fp;
}
