#include "../../include/utils/utils.h"

void print_output(const char *type, const char *text) {
	printf("%s: %s%c", type, text, NEWLINE);
	fflush(stdout);
}

void msleep(int ms) {
	struct timeval timeval;

	timeval.tv_sec = ms / MILLIS_PER_SECOND;
	timeval.tv_usec = (ms % MILLIS_PER_SECOND) * MICROSECONDS_PER_MILLISECOND;

	select(0, NULL, NULL, NULL, &timeval);
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
	ssize_t ret = write(fd, buf, count);

	if (ret < 0) {
		print_output("ERROR", strerror(errno));
	}
	return ret;
}

bool is_whitespace_only(const char *str) {
	while (*str) {
		if (!isspace((unsigned char)*str)) {
			return false;
		}
		str++;
	}
	return true;
}

char *trim_whitespace(char *str) {
	char *end;

	while (isspace((unsigned char)*str)) { // Trim leading space
		str++;
	}
	if (*str == 0) {
		return str;
	}
	end = str + strlen(str) - 1; // Trim trailing space

	while (end > str && isspace((unsigned char)*end)) {
		end--;
	}
	*(end + 1) = 0; // Write new null terminator

	return str;
}
