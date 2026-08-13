#include "../../include/utils/utils.h"

void print_output(const char *type, const char *text) {
	if (timeless_mode) {
		if (strcmp(type, MSG_TYPE_USER) == 0) {
			printf("%s %s%c", type, text, NEWLINE);

		} else {
			printf("%s: %s%c", type, text, NEWLINE);
		}
	} else {
		time_t now = time(NULL);
		struct tm tm_info;
		char time_buffer[24];

		localtime_r(&now, &tm_info);
		strftime(time_buffer, sizeof(time_buffer), "%Y/%m/%d %H:%M:%S", &tm_info);

		if (strcmp(type, MSG_TYPE_USER) == 0) {
			printf("%s %s %s%c", time_buffer, type, text, NEWLINE);

		} else {
			printf("%s %s: %s%c", time_buffer, type, text, NEWLINE);
		}
	}
	fflush(stdout);
}

void msleep(int millis) {
	struct timespec req, rem;

	req.tv_sec = millis / MILLIS_PER_SECOND;
	req.tv_nsec = (millis % MILLIS_PER_SECOND) * NANOSECONDS_PER_MILLIS;

	while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
		req = rem;
	}
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
	ssize_t ret = write(fd, buf, count);

	if (ret < 0) {
		/* Ignore non-blocking flow control errors */
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			print_output("ERROR", strerror(errno));
		}
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

	while (isspace((unsigned char)*str)) { /* Trim leading space */
		str++;
	}
	if (*str == 0) {
		return str;
	}
	end = str + strlen(str) - 1; /* Trim trailing space */

	while (end > str && isspace((unsigned char)*end)) {
		end--;
	}
	*(end + 1) = 0; /* Write new null terminator */

	return str;
}
