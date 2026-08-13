#include "../../include/modem/parser.h"

/* Inner STATIC methods */
/* ==================================================================== */
static bool is_error_ok_command(const char *cmd) {
	if (!cmd) {
		return false;
	}
	return strstr(cmd, "AT+CHUP") != NULL;
}

/* Outer methods */
/* ==================================================================== */
void categorize_line(const char *line) {
	if (strlen(line) == 0 || is_whitespace_only(line)) {
		return;
	}
	/* SMS notifications */
	if (is_sms_message(line)) {
		print_output(MSG_TYPE_SMS, line);
	}
	/* Call notifications */
	else if (is_call_message(line)) {
		handle_call_message(line);
	}
	/* CME / CMS errors */
	else if (is_error_message(line)) {
		print_output(MSG_TYPE_ERROR, line);
	}
	/* Command is valid */
	else if (is_ok_response(line)) {
		/* Suppress OK responses after call commands to reduce spam */
		if (!call_state.call_in_progress || !is_error_ok_command(terminal.last_command)) {
			print_output(MSG_TYPE_COMPLETE, "OK");
		}
	}
	/* Generic errors */
	else if (strstr(line, MSG_TYPE_ERROR)) {
		/* False positive errors */
		if (is_error_ok_command(terminal.last_command)) {
			print_output(MSG_TYPE_COMPLETE, "OK");

		} else {
			print_output(MSG_TYPE_ERROR, line);
		}
	}
	/* SIM card errors */
	else if (is_sim_error_message(line)) {
		print_output(MSG_TYPE_ERROR, "SIMCARD MISSING");

	} else {
		print_output(MSG_TYPE_RESPONSE, line);
	}
}
