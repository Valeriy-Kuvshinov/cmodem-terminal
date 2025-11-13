#include "../../include/threads/threads.h"

/* Inner STATIC methods */
/* ==================================================================== */
static bool sanitize_input(char *line, size_t buffer_size) {
	line[buffer_size - 1] = NULL_TERMINATOR;

	if (!HAS_NEWLINE(line) && IS_BUFFER_FULL(line, buffer_size))
		return false;

	size_t length = strlen(line);

	if (length > 0 && line[length - 1] == NEWLINE)
		line[length - 1] = NULL_TERMINATOR;

	return true;
}

static void clear_stdin_buffer(void) {
	int c;

	while (READ_UNTIL_NEWLINE_OR_EOF(c))
		;
}

static void record_last_command(const char *cmd) {
	strncpy(terminal.last_command, cmd, sizeof(terminal.last_command) - 1);

	terminal.last_command[sizeof(terminal.last_command) - 1] = NULL_TERMINATOR;
}

static void complete_sms_sending() {
	char sms_end_marker = CTRL_Z;

	pthread_mutex_lock(&terminal.serial_mutex);

	safe_write(terminal.fd, &sms_end_marker, 1);

	pthread_mutex_unlock(&terminal.serial_mutex);
}

static void send_sms_content(const char *line) {
	pthread_mutex_lock(&terminal.serial_mutex);

	safe_write(terminal.fd, line, strlen(line));

	pthread_mutex_unlock(&terminal.serial_mutex);
}

static void send_raw_command(const char *cmd, const char *record_cmd) {
	pthread_mutex_lock(&terminal.serial_mutex);

	if (record_cmd)
		record_last_command(record_cmd);
	else
		record_last_command(cmd);

	safe_write(terminal.fd, cmd, strlen(cmd));
	safe_write(terminal.fd, CRLF, CRLF_LENGTH);

	pthread_mutex_unlock(&terminal.serial_mutex);
}

static void send_command(const char *line) { send_raw_command(line, NULL); }

static void send_sms_command(const char *line) {
	send_raw_command(line, AT_SEND_SMS);
}

static int process_line(char *line, int sms_mode) {
	if (IS_EXIT_COMMAND(line)) {
		print_output(MSG_TYPE_STATUS, "Shutting down...");

		set_terminal_running(false);

		return EXIT_SIGNAL;
	} else if (IS_SMS_COMMAND(line)) {
		send_sms_command(line);
		msleep(SMS_SEND_DELAY_MS);

		return SMS_MODE_ON;
	}

	if (sms_mode) {
		send_sms_content(line);
		complete_sms_sending();
		msleep(COMMAND_DELAY_MS);

		return SMS_MODE_OFF;
	} else {
		send_command(line);
		msleep(COMMAND_DELAY_MS);

		return SMS_MODE_OFF;
	}
}

/* Outer methods */
/* ==================================================================== */
void *read_stdin_thread(void *arg) {
	char line[MAX_COMMAND];
	int sms_mode = false;

	while (terminal.is_running) {
		if (fgets(line, sizeof(line), stdin) != NULL) {
			if (!sanitize_input(line, sizeof(line))) {
				clear_stdin_buffer();

				print_output(MSG_TYPE_WARNING, "Input too long - ignored");

				continue;
			}
			if (strlen(line) > 0) {
				int new_mode = process_line(line, sms_mode);

				if (new_mode == EXIT_SIGNAL)
					break;

				sms_mode = new_mode;
			}
		}
	}
	pthread_exit(NULL);
}
