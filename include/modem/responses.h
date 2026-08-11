#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdbool.h>
#include <string.h>

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

/* Function prototypes */
/* For message categorization */
bool is_sms_message(const char *line);
bool is_ring_message(const char *line);
bool is_call_id_message(const char *line);
bool is_missed_call_message(const char *line);
bool is_call_begin_message(const char *line);
bool is_call_end_message(const char *line);
bool is_call_message(const char *line);
bool is_error_message(const char *line);
bool is_sim_error_message(const char *line);
bool is_urgent_message(const char *line);
bool is_ok_response(const char *line);

#endif