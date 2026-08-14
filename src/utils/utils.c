#include "../../include/utils/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void generate_timestamp(char *buffer, size_t size) {
	time_t now = time(NULL);
	struct tm tm_info;

	localtime_r(&now, &tm_info);
	strftime(buffer, size, "%Y/%m/%d %H:%M:%S", &tm_info);
}

static void print_user_message(const char *time_prefix, const char *text) {
	if (remote_mode) {
		return; /* Prevent duplicate log entries in server environments	*/
	}
	if (time_prefix) {
		printf("%s%s%s %s %s%c", ANSI_CURSOR_UP, ANSI_ERASE_LINE, time_prefix, MSG_TYPE_USER, text,
			   NEWLINE);
	} else {
		printf("%s%s%s %s%c", ANSI_CURSOR_UP, ANSI_ERASE_LINE, MSG_TYPE_USER, text, NEWLINE);
	}
}

static void print_system_message(const char *time_prefix, const char *type, const char *text) {
	if (time_prefix) {
		printf("%s %s: %s%c", time_prefix, type, text, NEWLINE);
	} else {
		printf("%s: %s%c", type, text, NEWLINE);
	}
}

/* Outer methods */
/* ==================================================================== */
void print_output(const char *type, const char *text) {
	char time_buffer[24] = {0};
	char *time_ptr = NULL;

	if (!timeless_mode) {
		generate_timestamp(time_buffer, sizeof(time_buffer));
		time_ptr = time_buffer;
	}
	if (strcmp(type, MSG_TYPE_USER) == 0) {
		print_user_message(time_ptr, text);

	} else {
		print_system_message(time_ptr, type, text);
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
