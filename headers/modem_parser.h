#ifndef MODEM_PARSER_H
#define MODEM_PARSER_H

#include "modem.h"

/* Modem response patterns */
#define URGENT_MSG_CMTI "+CMTI"
#define URGENT_MSG_CMT "+CMT"
#define URGENT_MSG_RING "RING"
#define URGENT_MSG_CRING "+CRING"
#define URGENT_MSG_CLIP "+CLIP"
#define URGENT_MSG_COLP "+COLP"
#define URGENT_MSG_VOICE_CALL_BEGIN "VOICE CALL: BEGIN"
#define URGENT_MSG_VOICE_CALL_END "VOICE CALL: END"
#define URGENT_MSG_NO_CARRIER "NO CARRIER"
#define URGENT_MSG_BUSY "BUSY"
#define URGENT_MSG_NO_ANSWER "NO ANSWER"
#define URGENT_MSG_CME_ERROR "+CME ERROR"
#define URGENT_MSG_CMS_ERROR "+CMS ERROR"
#define URGENT_MSG_SIMCARD_NOT_AVAILABLE "+SIMCARD: NOT AVAILABLE"
#define URGENT_MSG_NOT_REGISTERED "+CREG: 0"

/* Macros for message categorization */
#define IS_CALL_END_MESSAGE(line)                                              \
  (strstr(line, URGENT_MSG_NO_CARRIER) || strstr(line, URGENT_MSG_BUSY) ||     \
   strstr(line, URGENT_MSG_NO_ANSWER) ||                                       \
   strstr(line, URGENT_MSG_VOICE_CALL_END))

#define IS_CALL_BEGIN_MESSAGE(line) (strstr(line, URGENT_MSG_VOICE_CALL_BEGIN))

#define IS_SMS_MESSAGE(line)                                                   \
  (strstr(line, URGENT_MSG_CMTI) || strstr(line, URGENT_MSG_CMT))

#define IS_CALL_MESSAGE(line)                                                  \
  (strstr(line, URGENT_MSG_RING) || strstr(line, URGENT_MSG_CRING) ||          \
   strstr(line, URGENT_MSG_CLIP) || strstr(line, URGENT_MSG_COLP) ||           \
   strstr(line, URGENT_MSG_NO_CARRIER) || strstr(line, URGENT_MSG_BUSY) ||     \
   strstr(line, URGENT_MSG_NO_ANSWER) ||                                       \
   strstr(line, URGENT_MSG_VOICE_CALL_END) ||                                  \
   strstr(line, URGENT_MSG_VOICE_CALL_BEGIN))

#define IS_ERROR_MESSAGE(line)                                                 \
  (strstr(line, URGENT_MSG_CME_ERROR) || strstr(line, URGENT_MSG_CMS_ERROR))

#define IS_SIM_ERROR_MESSAGE(line)                                             \
  (strstr(line, URGENT_MSG_SIMCARD_NOT_AVAILABLE) ||                           \
   strstr(line, URGENT_MSG_NOT_REGISTERED))

#define IS_ERROR_OK_COMMAND(cmd) (strstr(cmd, ERROR_OK_AT_CHUP))

#define IS_OK_RESPONSE(line) (strcmp(line, MODEM_AT_RESPONSE_OK) == 0)

int check_urgent_message(const char *buffer);
void categorize_and_output_line(const ModemTerminal *term, const char *line);

#endif