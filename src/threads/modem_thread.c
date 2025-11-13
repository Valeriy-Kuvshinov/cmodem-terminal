#include "../../include/threads/threads.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void handle_remaining_buffer(char *line_start) {
	if (IS_URGENT_MESSAGE(line_start)) {
		print_output(MSG_TYPE_URGENT, line_start);

		line_start += strlen(line_start);
	}
	terminal.buffer_length = strlen(line_start);

	if (terminal.buffer_length > 0 && line_start != terminal.output_buffer)
		memmove(terminal.output_buffer, line_start, terminal.buffer_length + 1);

	else if (terminal.buffer_length == 0)
		terminal.output_buffer[0] = NULL_TERMINATOR;
}

static void add_to_buffer(const char *data, int length) {
	if (terminal.buffer_length + length < MAX_BUFFER - 1) {
		memcpy(terminal.output_buffer + terminal.buffer_length, data, length);

		terminal.buffer_length += length;
		terminal.output_buffer[terminal.buffer_length] = NULL_TERMINATOR;
	}
}

static void process_complete_lines(void) {
	char *crlf;
	char *line_start = terminal.output_buffer;

	while ((crlf = strstr(line_start, CRLF)) != NULL) {
		*crlf = NULL_TERMINATOR;

		if (strlen(line_start) > 0)
			categorize_line(line_start);

		line_start = crlf + CRLF_LENGTH;
	}
	handle_remaining_buffer(line_start);
}

static void process_received_data(char *temp_buf, int n) {
	temp_buf[n] = NULL_TERMINATOR;

	pthread_mutex_lock(&terminal.serial_mutex);
	add_to_buffer(temp_buf, n);

	process_complete_lines();
	pthread_mutex_unlock(&terminal.serial_mutex);
}

/* Outer methods */
/* ==================================================================== */
bool init_terminal(const char *device_port) {
	memset(&terminal, 0, sizeof(ModemTerminal));

	terminal.is_running = true;

	pthread_mutex_init(&terminal.serial_mutex, NULL);
	pthread_mutex_init(&terminal.running_mutex, NULL);

	terminal.fd =
		open_serial_port(device_port, MAX_PORT_RETRIES, PORT_RETRY_DELAY_SEC);

	if (terminal.fd < 0) {
		pthread_mutex_destroy(&terminal.serial_mutex);
		pthread_mutex_destroy(&terminal.running_mutex);

		return false;
	}
	return true;
}

void *read_modem_thread(void *arg) {
	char temp_buf[MAX_BUFFER];

	while (terminal.is_running) {
		int n;

		n = read(terminal.fd, temp_buf, sizeof(temp_buf) - 1);

		if (n > 0)
			process_received_data(temp_buf, n);

		msleep(THREAD_SLEEP_MS);
	}
	pthread_exit(NULL);
}