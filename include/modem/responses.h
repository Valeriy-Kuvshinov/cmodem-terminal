#ifndef RESPONSES_H
#define RESPONSES_H

/* Update message types */
#define MSG_TYPE_STATUS "[STATUS]"
#define MSG_TYPE_ERROR "[ERROR]"
#define MSG_TYPE_COMPLETE "[COMPLETE]"
#define MSG_TYPE_INFO "[INFO]"
#define MSG_TYPE_RESPONSE "[RESPONSE]"
#define MSG_TYPE_WARNING "[WARNING]"
#define MSG_TYPE_URGENT "[URGENT]"
#define MSG_TYPE_SMS "[SMS]"
#define MSG_TYPE_CALL "[CALL]"

/* Macros for message categorization */
#define IS_SMS_MESSAGE(line) (strstr(line, "+CMTI") || strstr(line, "+CMT"))

#define IS_RING_MESSAGE(line) (strstr(line, "RING") || strstr(line, "+CRING"))

#define IS_CALL_ID_MESSAGE(line) (strstr(line, "+CLIP"))

#define IS_MISSED_CALL_MESSAGE(line) (strstr(line, "MISSED_CALL"))

#define IS_CALL_BEGIN_MESSAGE(line) (strstr(line, "VOICE CALL: BEGIN"))

#define IS_CALL_END_MESSAGE(line)                                              \
	(strstr(line, "NO CARRIER") || strstr(line, "BUSY") ||                     \
	 strstr(line, "NO ANSWER") || strstr(line, "VOICE CALL: END"))

#define IS_CALL_MESSAGE(line)                                                  \
	(IS_RING_MESSAGE(line) || IS_CALL_ID_MESSAGE(line) ||                      \
	 IS_CALL_END_MESSAGE(line) || IS_CALL_BEGIN_MESSAGE(line) ||               \
	 IS_MISSED_CALL_MESSAGE(line))

#define IS_ERROR_MESSAGE(line)                                                 \
	(strstr(line, "+CME ERROR") || strstr(line, "+CMS ERROR") ||               \
	 strstr(line, "ERROR"))

#define IS_SIM_ERROR_MESSAGE(line)                                             \
	(strstr(line, "+SIMCARD: NOT AVAILABLE") || strstr(line, "+CREG: 0"))

#define IS_URGENT_MESSAGE(line)                                                \
	(IS_SMS_MESSAGE(line) || IS_CALL_MESSAGE(line) ||                          \
	 IS_ERROR_MESSAGE(line) || IS_SIM_ERROR_MESSAGE(line))

#define IS_OK_RESPONSE(line) (strcmp(line, "OK") == 0)

#endif