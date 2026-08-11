#include <signal.h>

#include "../include/config.h"
#include "../include/modem/call.h"
#include "../include/threads/threads.h"
#include "../include/utils/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static bool has_quiet_mode_flag(int argc, char *argv[]) {
	return (argc == 3) && (strcmp(argv[2], QUIET_MODE_FLAG) == 0);
}

static bool is_args_valid(int argc, char *argv[], int *quiet_mode) {
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s <device_port> [%s]%c", argv[0], QUIET_MODE_FLAG, NEWLINE);
		return false;
	}
	if (argc == 3 && !has_quiet_mode_flag(argc, argv)) {
		fprintf(stderr, "Usage: %s <device_port> [%s]%c", argv[0], QUIET_MODE_FLAG, NEWLINE);
		return false;
	}
	*quiet_mode = has_quiet_mode_flag(argc, argv);

	return true;
}

static void signal_handler(int signum) { set_terminal_running(false); }

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

	if (!is_args_valid(argc, argv, &quiet_mode)) {
		return 1;
	}
	device_port = argv[1];

	if (!load_config()) {
		printf("Failed to load %s%c", CONFIG_FILE_NAME, NEWLINE);
		return 1;
	}
	if (!init_terminal(device_port)) {
		return 1;
	}
	if (!init_modem()) {
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
