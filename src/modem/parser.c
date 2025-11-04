#include "../../include/modem/parser.h"

// Call state tracking to reduce spam
static bool call_in_progress = false;
static char last_caller_id[32] = {0};
static time_t last_ring_time = 0;

/* Inner STATIC methods */
/* ==================================================================== */
static bool is_duplicate_ring_msg(const char *line) {
  time_t current_time = time(NULL);

  if (IS_RING_MESSAGE(line)) {
    if (current_time - last_ring_time < 2) // Within 2 seconds
      return true;

    last_ring_time = current_time;

    return false;
  }
  return false;
}

static void handle_call_end(const char *line) {
  if (call_in_progress) {
    call_in_progress = false;
    last_caller_id[0] = NULL_TERMINATOR;
  }
}

static void handle_call_begin(const char *line) {
  if (!call_in_progress)
    call_in_progress = true;
}

static void extract_caller_id(const char *line, char *caller_id,
                              size_t max_len) {
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

  if (len >= max_len)
    return;

  strncpy(caller_id, start, len);

  caller_id[len] = NULL_TERMINATOR;
}

static void handle_call_msg(const char *line) {
  if (IS_CALL_ID_MESSAGE(line))
    extract_caller_id(line, last_caller_id, sizeof(last_caller_id));

  if (!is_duplicate_ring_msg(line))
    print_output(MSG_TYPE_CALL, line);

  if (IS_CALL_BEGIN_MESSAGE(line))
    handle_call_begin(line);

  else if (IS_CALL_END_MESSAGE(line))
    handle_call_end(line);
}

/* Outer methods */
/* ==================================================================== */
void categorize_line(const ModemTerminal *term, const char *line) {
  if (strlen(line) == 0 || is_whitespace_only(line))
    return;

  /* SMS notifications */
  if (IS_SMS_MESSAGE(line))
    print_output(MSG_TYPE_SMS, line);

  /* Call notifications */
  else if (IS_CALL_MESSAGE(line))
    handle_call_msg(line);

  /* CME / CMS errors */
  else if (IS_ERROR_MESSAGE(line))
    print_output(MSG_TYPE_ERROR, line);

  /* Command is valid */
  else if (IS_OK_RESPONSE(line)) {
    // Suppress OK responses after call commands to reduce spam
    if (!call_in_progress || !IS_ERROR_OK_COMMAND(term->last_command))
      print_output(MSG_TYPE_COMPLETE, MODEM_RESPONSE_OK);
  }

  /* Generic errors */
  else if (strstr(line, MSG_TYPE_ERROR)) {
    /* False positive errors */
    if (IS_ERROR_OK_COMMAND(term->last_command))
      print_output(MSG_TYPE_COMPLETE, MODEM_RESPONSE_OK);

    else
      print_output(MSG_TYPE_ERROR, line);
  }
  /* SIM card errors */
  else if (IS_SIM_ERROR_MESSAGE(line))
    print_output(MSG_TYPE_ERROR, "SIMCARD MISSING");

  else
    print_output(MSG_TYPE_RESPONSE, line);
}