#include <string.h>

#include "../../include/modem/parser.h"

static const char *urgent_msg_arr[] = {URGENT_MSG_CMTI,
                                       URGENT_MSG_CMT,
                                       URGENT_MSG_RING,
                                       URGENT_MSG_CRING,
                                       URGENT_MSG_CLIP,
                                       URGENT_MSG_VOICE_CALL_BEGIN,
                                       URGENT_MSG_VOICE_CALL_END,
                                       URGENT_MSG_NO_CARRIER,
                                       URGENT_MSG_BUSY,
                                       URGENT_MSG_NO_ANSWER,
                                       URGENT_MSG_CME_ERROR,
                                       URGENT_MSG_CMS_ERROR,
                                       URGENT_MSG_SIMCARD_NOT_AVAILABLE,
                                       NULL};

/* Inner STATIC methods */
/* ==================================================================== */
static void handle_call_end_msg(const char *line) {
  char msg[MAX_BUFFER];

  snprintf(msg, sizeof(msg), "CALL_END: %s", line);
  print_output(MSG_TYPE_COMPLETE, msg);
}

static void handle_call_begin_msg(const char *line) {
  char msg[MAX_BUFFER];

  snprintf(msg, sizeof(msg), "CALL_CONNECTED: %s", line);
  print_output(MSG_TYPE_COMPLETE, msg);
}

static void handle_call_msg(const char *line) {
  print_output(MSG_TYPE_CALL, line);

  if (IS_CALL_END_MESSAGE(line))
    handle_call_end_msg(line);

  else if (IS_CALL_BEGIN_MESSAGE(line))
    handle_call_begin_msg(line);
}

/* Outer methods */
/* ==================================================================== */
int check_urgent_message(const char *buffer) {
  int i;

  for (i = 0; urgent_msg_arr[i] != NULL; i++) {
    if (strstr(buffer, urgent_msg_arr[i]) != NULL)
      return 1;
  }
  return 0;
}

void categorize_line(const ModemTerminal *term, const char *line) {
  if (strlen(line) == 0)
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
  else if (IS_OK_RESPONSE(line))
    print_output(MSG_TYPE_COMPLETE, MODEM_RESPONSE_OK);

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