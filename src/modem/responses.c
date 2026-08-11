#include "../../include/modem/responses.h"

/* Outer methods */
/* ==================================================================== */
bool is_sms_message(const char *line) { return strstr(line, "+CMTI") || strstr(line, "+CMT"); }

bool is_ring_message(const char *line) { return strstr(line, "RING") || strstr(line, "+CRING"); }

bool is_call_id_message(const char *line) { return strstr(line, "+CLIP") != NULL; }

bool is_missed_call_message(const char *line) { return strstr(line, "MISSED_CALL") != NULL; }

bool is_call_begin_message(const char *line) { return strstr(line, "VOICE CALL: BEGIN") != NULL; }

bool is_call_end_message(const char *line) {
	return strstr(line, "NO CARRIER") || strstr(line, "BUSY") || strstr(line, "NO ANSWER") ||
		   strstr(line, "VOICE CALL: END");
}

bool is_call_message(const char *line) {
	return is_ring_message(line) || is_call_id_message(line) || is_call_end_message(line) ||
		   is_call_begin_message(line) || is_missed_call_message(line);
}

bool is_error_message(const char *line) {
	return strstr(line, "+CME ERROR") || strstr(line, "+CMS ERROR") || strstr(line, "ERROR");
}

bool is_sim_error_message(const char *line) {
	return strstr(line, "+SIMCARD: NOT AVAILABLE") || strstr(line, "+CREG: 0") ||
		   strstr(line, "SIM not inserted");
}

bool is_urgent_message(const char *line) {
	return is_sms_message(line) || is_call_message(line) || is_error_message(line) ||
		   is_sim_error_message(line);
}

bool is_ok_response(const char *line) { return strstr(line, "OK") != NULL; }