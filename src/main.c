#include "../include/config.h"
#include "../include/modem/call.h"
#include "../include/threads/threads.h"
#include "../include/utils/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void print_usage(const char *program_name) {
	fprintf(stderr, "Usage: %s <device_port> [%s] [%s] [%s]%c", program_name, QUIET_MODE_FLAG,
			TIMELESS_MODE_FLAG, REMOTE_MODE_FLAG, NEWLINE);
}

static bool parse_single_flag(const char *flag, int *quiet_mode) {
	if (strcmp(flag, QUIET_MODE_FLAG) == 0) {
		*quiet_mode = 1;
		return true;
	}
	if (strcmp(flag, TIMELESS_MODE_FLAG) == 0) {
		timeless_mode = true;
		return true;
	}
	if (strcmp(flag, REMOTE_MODE_FLAG) == 0) {
		remote_mode = true;
		return true;
	}
	return false;
}

static bool is_args_valid(int argc, char *argv[], int *quiet_mode) {
	int i;

	if (argc < MIN_APP_ARGUMENTS || argc > MAX_APP_ARGUMENTS) {
		print_usage(argv[0]);
		return false;
	}
	*quiet_mode = 0;
	timeless_mode = false;
	remote_mode = false;

	/* Iterate over optional flags */
	for (i = MIN_APP_ARGUMENTS; i < argc; i++) {
		if (!parse_single_flag(argv[i], quiet_mode)) {
			fprintf(stderr, "Invalid argument: %s%c", argv[i], NEWLINE);
			print_usage(argv[0]);

			return false;
		}
	}
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
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	if (!init_modem()) {
		cleanup_terminal();
		return 1;
	}
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
