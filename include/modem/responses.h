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

#define MODEM_RESPONSE_OK "OK"

/* Modem response patterns */
#define URGENT_MSG_CMTI "+CMTI"
#define URGENT_MSG_CMT "+CMT"
#define URGENT_MSG_RING "RING"
#define URGENT_MSG_CRING "+CRING"
#define URGENT_MSG_CLIP "+CLIP"
#define URGENT_MSG_VOICE_CALL_BEGIN "VOICE CALL: BEGIN"
#define URGENT_MSG_VOICE_CALL_END "VOICE CALL: END"
#define URGENT_MSG_MISSED_CALL "MISSED_CALL:"
#define URGENT_MSG_NO_CARRIER "NO CARRIER"
#define URGENT_MSG_BUSY "BUSY"
#define URGENT_MSG_NO_ANSWER "NO ANSWER"
#define URGENT_MSG_CME_ERROR "+CME ERROR"
#define URGENT_MSG_CMS_ERROR "+CMS ERROR"
#define URGENT_MSG_SIMCARD_NOT_AVAILABLE "+SIMCARD: NOT AVAILABLE"
#define URGENT_MSG_NOT_REGISTERED "+CREG: 0"

/* Macros for message categorization */
#define IS_SMS_MESSAGE(line)                                                   \
	(strstr(line, URGENT_MSG_CMTI) || strstr(line, URGENT_MSG_CMT))

#define IS_RING_MESSAGE(line)                                                  \
	(strstr(line, URGENT_MSG_RING) || strstr(line, URGENT_MSG_CRING))

#define IS_CALL_ID_MESSAGE(line) (strstr(line, URGENT_MSG_CLIP))

#define IS_MISSED_CALL_MESSAGE(line) (strstr(line, URGENT_MSG_MISSED_CALL))

#define IS_CALL_BEGIN_MESSAGE(line) (strstr(line, URGENT_MSG_VOICE_CALL_BEGIN))

#define IS_CALL_END_MESSAGE(line)                                              \
	(strstr(line, URGENT_MSG_NO_CARRIER) || strstr(line, URGENT_MSG_BUSY) ||   \
	 strstr(line, URGENT_MSG_NO_ANSWER) ||                                     \
	 strstr(line, URGENT_MSG_VOICE_CALL_END))

#define IS_CALL_MESSAGE(line)                                                  \
	(IS_RING_MESSAGE(line) || IS_CALL_ID_MESSAGE(line) ||                      \
	 IS_CALL_END_MESSAGE(line) || IS_CALL_BEGIN_MESSAGE(line) ||               \
	 IS_MISSED_CALL_MESSAGE(line))

#define IS_ERROR_MESSAGE(line)                                                 \
	(strstr(line, URGENT_MSG_CME_ERROR) || strstr(line, URGENT_MSG_CMS_ERROR))

#define IS_SIM_ERROR_MESSAGE(line)                                             \
	(strstr(line, URGENT_MSG_SIMCARD_NOT_AVAILABLE) ||                         \
	 strstr(line, URGENT_MSG_NOT_REGISTERED))

#define IS_URGENT_MESSAGE(line)                                                \
	(IS_SMS_MESSAGE(line) || IS_CALL_MESSAGE(line) ||                          \
	 IS_ERROR_MESSAGE(line) || IS_SIM_ERROR_MESSAGE(line))

#define IS_OK_RESPONSE(line) (strcmp(line, MODEM_RESPONSE_OK) == 0)

#endif