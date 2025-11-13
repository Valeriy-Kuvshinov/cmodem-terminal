#include "../../include/modem/call.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void handle_call_end(const char *line) {
	if (call_state.call_in_progress) {
		call_state.call_in_progress = false;
		call_state.last_caller_id[0] = NULL_TERMINATOR;
	}
}

static void handle_call_begin(const char *line) {
	if (!call_state.call_in_progress)
		call_state.call_in_progress = true;
}

static void extract_caller_id(const char *line, char *caller_id) {
	char *comma, *start, *end;

	comma = strchr(line, ',');

	if (!comma)
		return;

	start = comma - 1;

	// Find the opening quote
	while (start > line && *start != '"')
		start--;

	if (*start != '"')
		return;

	start++;
	end = strchr(start, '"');

	if (!end)
		return;

	size_t len = end - start;

	if (len >= CALLER_ID_LENGTH)
		return;

	strncpy(caller_id, start, len);

	caller_id[len] = NULL_TERMINATOR;
}

/* Outer methods */
/* ==================================================================== */
void init_call_state(void) {
	call_state.call_in_progress = false;
	call_state.last_caller_id[0] = NULL_TERMINATOR;
}

void handle_call_message(const char *line) {
	if (IS_CALL_ID_MESSAGE(line))
		extract_caller_id(line, call_state.last_caller_id);

	print_output(MSG_TYPE_CALL, line);

	if (IS_CALL_BEGIN_MESSAGE(line))
		handle_call_begin(line);

	else if (IS_CALL_END_MESSAGE(line))
		handle_call_end(line);
}

void cleanup_call_state(void) { memset(&call_state, 0, sizeof(CallState)); }