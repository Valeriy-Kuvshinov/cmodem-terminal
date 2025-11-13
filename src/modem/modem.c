#include "../../include/modem/modem.h"

static const char *init_commands[][2] = {
	{AT_RESET, AT_RESET_DESC},
	{AT_NUMERIC_ERRORS_ON, AT_NUMERIC_ERRORS_ON_DESC},
	{AT_CHARACTERS_SET_UCS2, AT_CHARACTERS_SET_UCS2_DESC},
	{AT_SMS_PDU_ON, AT_SMS_PDU_ON_DESC},
	{AT_STORAGE_ME, AT_STORAGE_ME_DESC},
	{AT_NEW_NOTIFICATIONS, AT_NEW_NOTIFICATIONS_DESC},
	{AT_CLIP_ON, AT_CLIP_ON_DESC},
	{AT_EXTENDED_RING_ON, AT_EXTENDED_RING_ON_DESC},
	{AT_NETWORK_REGISTRATION_BASIC, AT_NETWORK_REGISTRATION_BASIC_DESC},
	{NULL, NULL}};

/* Inner STATIC methods */
/* ==================================================================== */
static void log_failure_final(const char *desc, const char *response) {
	char status[MAX_STATUS_MSG];
	char msg[MAX_STATUS_MSG]; // Temporary buffer for safe truncation

	if (strlen(response) > 0) {
		strncpy(msg, response, sizeof(msg) - 1);

		msg[sizeof(msg) - 1] = NEWLINE;
	} else
		strcpy(msg, "No response");

	snprintf(status, sizeof(status),
			 "%s failed after %d attempts (Response: %s)", desc,
			 MAX_INIT_RETRIES, msg);
	print_output(MSG_TYPE_WARNING, status);
}

static int send_init_command(const char *cmd, char *response,
							 size_t response_size) {
	int bytes_read;

	pthread_mutex_lock(&terminal.serial_mutex);

	safe_write(terminal.fd, cmd, strlen(cmd));
	safe_write(terminal.fd, CRLF, CRLF_LENGTH);

	pthread_mutex_unlock(&terminal.serial_mutex);

	msleep(MODEM_RESPONSE_DELAY_MS);

	bytes_read = read(terminal.fd, response, response_size - 1);

	if (bytes_read > 0)
		response[bytes_read] = NULL_TERMINATOR;

	return bytes_read;
}

static void log_init_command(const char *desc, int attempt) {
	char status[MAX_STATUS_MSG];

	if (attempt > 0)
		snprintf(status, sizeof(status), "%s (retry %d/%d)", desc, attempt + 1,
				 MAX_INIT_RETRIES);

	else
		snprintf(status, sizeof(status), "%s", desc);

	print_output(MSG_TYPE_STATUS, status);
}

static void log_success(const char *desc, const char *response) {
	char status[MAX_STATUS_MSG];

	snprintf(status, sizeof(status), "%s returned: %s", desc, response);
	print_output(MSG_TYPE_INFO, status);
}

static void log_failure(const char *desc, int attempt, const char *response) {
	char status[MAX_STATUS_MSG];
	const char *msg;

	msg = strlen(response) > 0 ? response : "No response";

	snprintf(status, sizeof(status),
			 "%s failed, retrying in %d seconds... (Response: %s)", desc,
			 INIT_RETRY_DELAY_SEC, msg);
	print_output(MSG_TYPE_WARNING, status);
}

static bool process_response(const char *response, int bytes_read, int attempt,
							 const char *desc) {
	if (IS_OK_RESPONSE(response))
		return true;

	else if (bytes_read == 0 || IS_ERROR_MESSAGE(response)) {
		if (attempt < MAX_INIT_RETRIES - 1) {
			log_failure(desc, attempt, response);

			sleep(INIT_RETRY_DELAY_SEC);
		} else
			log_failure_final(desc, response);

		return false;
	} else {
		log_success(desc, response);

		return true;
	}
}

static bool run_init_command(const char *cmd, const char *desc) {
	int attempt;

	for (attempt = 0; attempt < MAX_INIT_RETRIES; attempt++) {
		char response[MAX_BUFFER] = {0};
		int bytes_read;

		log_init_command(desc, attempt);

		bytes_read = send_init_command(cmd, response, sizeof(response));

		if (process_response(response, bytes_read, attempt, desc))
			return true;
	}
	return false;
}

/* Outer methods */
/* ==================================================================== */
bool init_modem(void) {
	int i;

	for (i = 0; init_commands[i][0] != NULL; i++) {
		const char *cmd = init_commands[i][0];
		const char *desc = init_commands[i][1];

		if (run_init_command(cmd, desc))
			msleep(MODEM_RESPONSE_DELAY_MS);
	}
	print_output(MSG_TYPE_STATUS, "INIT COMPLETE");

	return true;
}