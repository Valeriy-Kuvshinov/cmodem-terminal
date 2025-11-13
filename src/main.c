#include <signal.h>

#include "../include/modem/call.h"
#include "../include/threads/threads.h"
#include "../include/utils/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static bool is_args_valid(int argc, char *argv[], int *quiet_mode) {
	if (argc < 2 || argc > 4) {
		fprintf(stderr, "Usage: %s <device_port> [%s]%c", argv[0],
				QUIET_MODE_FLAG, NEWLINE);

		return false;
	}
	*quiet_mode = HAS_QUIET_MODE_FLAG(argc, argv);

	if (argc == 3 && !HAS_QUIET_MODE_FLAG(argc, argv)) {
		fprintf(stderr, "Usage: %s <device_port> [%s]%c", argv[0],
				QUIET_MODE_FLAG, NEWLINE);

		return false;
	}
	return true;
}

static void signal_handler(int signum) { set_terminal_running(false); }

static bool is_connection_stable(int fd) {
	char response[MAX_RESPONSE];
	int n;

	print_output(MSG_TYPE_STATUS, "Testing modem connection...");

	safe_write(fd, AT_CRLF, AT_CRLF_LENGTH);
	sleep(2);

	n = read(fd, response, sizeof(response) - 1);

	if (n > 0) {
		response[n] = NULL_TERMINATOR;
		char debug_msg[200]; // Increase buffer size
		char truncated_response[50];
		strncpy(truncated_response, response, sizeof(truncated_response) - 1);
		truncated_response[sizeof(truncated_response) - 1] = '\0';

		snprintf(debug_msg, sizeof(debug_msg), "Received: '%s...' (length: %d)",
				 truncated_response, n);
		print_output(MSG_TYPE_INFO, debug_msg);

		if (strstr(response, "OK"))
			return true;
	} else {
		char debug_msg[50];
		snprintf(debug_msg, sizeof(debug_msg), "Read returned: %d", n);
		print_output(MSG_TYPE_INFO, debug_msg);
	}
	print_output(MSG_TYPE_ERROR, "No response to initial AT command");

	return false;
}

static void cleanup(void) {
	cleanup_call_state();
	cleanup_terminal();

	print_output(MSG_TYPE_STATUS, "Terminal has shut down");
}

/* App main method */
/* ==================================================================== */
int main(int argc, char *argv[]) {
	const char *device_port;
	int quiet_mode;
	pthread_t modem_thread, stdin_thread;

	if (!is_args_valid(argc, argv, &quiet_mode))
		return 1;

	device_port = argv[1];

	if (!init_terminal(device_port))
		return 1;

	if (!is_connection_stable(terminal.fd) || !init_modem()) {
		close(terminal.fd);

		return 1;
	}

	/* Register signal handlers for graceful shutdown */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	init_call_state();

	if (!quiet_mode) {
		printf("You may start writing AT commands.%c", NEWLINE);
		printf("Type '%s' to quit terminal.%c", EXIT_APP_COMMAND, NEWLINE);
	}

	start_threads(&modem_thread, &stdin_thread);

	exit_threads(modem_thread, stdin_thread);

	cleanup();

	return 0;
}
