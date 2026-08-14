#include "../../include/threads/threads.h"

static int current_sms_mode = SMS_MODE_OFF;

/* Keep track of commands to prevent consecutive duplicates */
static char last_history_cmd[MAX_COMMAND] = {0};

/* Inner STATIC methods */
/* ==================================================================== */
static bool is_exit_command(const char *line) { return strcasecmp(line, EXIT_APP_COMMAND) == 0; }

static bool is_sms_command(const char *line) {
	return strncmp(line, AT_SEND_SMS "=", AT_SEND_SMS_LENGTH + 1) == 0;
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

	if (record_cmd) {
		record_last_command(record_cmd);

	} else {
		record_last_command(cmd);
	}
	safe_write(terminal.fd, cmd, strlen(cmd));
	safe_write(terminal.fd, CRLF, CRLF_LENGTH);

	pthread_mutex_unlock(&terminal.serial_mutex);
}

static void send_command(const char *line) { send_raw_command(line, NULL); }

static void send_sms_command(const char *line) { send_raw_command(line, AT_SEND_SMS); }

static int process_line(char *line, int sms_mode) {
	if (is_exit_command(line)) {
		print_output(MSG_TYPE_COMPLETE, "Shutting down...");
		return EXIT_SIGNAL;
	}

	print_output(MSG_TYPE_USER, line);

	if (is_sms_command(line)) {
		send_sms_command(line);

		msleep(SMS_SEND_DELAY_MILLIS);

		return SMS_MODE_ON;
	}

	if (sms_mode) {
		send_sms_content(line);
		complete_sms_sending();

		msleep(COMMAND_DELAY_MILLIS);

		return SMS_MODE_OFF;

	} else {
		send_command(line);

		msleep(COMMAND_DELAY_MILLIS);

		return SMS_MODE_OFF;
	}
}

/* Callback triggered by Readline when the user presses Enter */
static void handle_input_line(char *line) {
	if (line == NULL) {
		/* EOF reached */
		print_output(MSG_TYPE_WARNING, "Standard input closed. Exiting terminal.");

		set_terminal_running(false);

		return;
	}

	if (strlen(line) >= MAX_COMMAND) {
		print_output(MSG_TYPE_WARNING, "Input too long - ignored");

		free(line);

		return;
	}
	if (strlen(line) > 0) {
		/* Prevent consecutive duplicate commands in history */
		if (strcmp(line, last_history_cmd) != 0) {
			add_history(line);
			strncpy(last_history_cmd, line, sizeof(last_history_cmd) - 1);
		}
		int new_mode = process_line(line, current_sms_mode);

		if (new_mode == EXIT_SIGNAL) {
			set_terminal_running(false);

		} else {
			current_sms_mode = new_mode;
		}
	}
	free(line);
}

/* Outer methods */
/* ==================================================================== */
void *read_stdin_thread(void *arg) {
	struct pollfd pfd;
	pfd.fd = STDIN_FILENO;
	pfd.events = POLLIN;

	stifle_history(MAX_COMMANDS_HISTORY);

	/* Initialize the Readline callback interface with an empty prompt */
	rl_callback_handler_install("", handle_input_line);

	while (atomic_load(&terminal.is_running)) {
		if (poll(&pfd, 1, STDIN_POLL_TIMEOUT_MILLIS) <= 0) {
			continue;
		}
		if (!(pfd.revents & POLLIN)) {
			continue;
		}
		rl_callback_read_char(); /* Delegate the raw character reading and editing to Readline */
	}
	rl_callback_handler_remove(); /* Restore terminal canonical mode upon exit */

	pthread_exit(NULL);
}
