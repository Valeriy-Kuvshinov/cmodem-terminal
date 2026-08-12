#include "../../include/threads/threads.h"

/* Outer methods */
/* ==================================================================== */
void set_terminal_running(bool value) { atomic_store(&terminal.is_running, value); }

void start_threads(pthread_t *modem_thread, pthread_t *stdin_thread) {
	pthread_create(modem_thread, NULL, read_modem_thread, NULL);
	pthread_create(stdin_thread, NULL, read_stdin_thread, NULL);
}

/* Wait for threads to finish their current operations */
void exit_threads(pthread_t modem_thread, pthread_t stdin_thread) {
	pthread_join(stdin_thread, NULL);
	pthread_join(modem_thread, NULL);
}

void cleanup_terminal(void) {
	if (terminal.fd >= 0) {
		tcflush(terminal.fd, TCIOFLUSH);

		close(terminal.fd);

		terminal.fd = -1;
	}
	// Clear sensitive buffers immediately
	memset(terminal.output_buffer, 0, sizeof(terminal.output_buffer));
	memset(terminal.last_command, 0, sizeof(terminal.last_command));

	pthread_mutex_destroy(&terminal.serial_mutex);
}
