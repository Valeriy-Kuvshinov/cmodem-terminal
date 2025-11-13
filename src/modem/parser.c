#include "../../include/modem/parser.h"
#include "../../include/modem/call.h"

/* Outer methods */
/* ==================================================================== */
void categorize_line(const char *line) {
	if (strlen(line) == 0 || is_whitespace_only(line))
		return;

	/* SMS notifications */
	if (IS_SMS_MESSAGE(line))
		print_output(MSG_TYPE_SMS, line);

	/* Call notifications */
	else if (IS_CALL_MESSAGE(line))
		handle_call_message(line);

	/* CME / CMS errors */
	else if (IS_ERROR_MESSAGE(line))
		print_output(MSG_TYPE_ERROR, line);

	/* Command is valid */
	else if (IS_OK_RESPONSE(line)) {
		// Suppress OK responses after call commands to reduce spam
		if (!call_state.call_in_progress ||
			!IS_ERROR_OK_COMMAND(terminal.last_command))
			print_output(MSG_TYPE_COMPLETE, "OK");
	}

	/* Generic errors */
	else if (strstr(line, MSG_TYPE_ERROR)) {
		/* False positive errors */
		if (IS_ERROR_OK_COMMAND(terminal.last_command))
			print_output(MSG_TYPE_COMPLETE, "OK");

		else
			print_output(MSG_TYPE_ERROR, line);
	}
	/* SIM card errors */
	else if (IS_SIM_ERROR_MESSAGE(line))
		print_output(MSG_TYPE_ERROR, "SIMCARD MISSING");

	else
		print_output(MSG_TYPE_RESPONSE, line);
}