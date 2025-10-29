#ifndef MODEM_H
#define MODEM_H

#include <pthread.h>

#include "../utils/utils.h"

typedef struct {
  int fd;
  int running;
  char output_buffer[MAX_BUFFER];
  size_t buffer_len;
  char last_command[MAX_COMMAND];
  pthread_mutex_t serial_mutex;
  pthread_mutex_t running_mutex;
} ModemTerminal;

#define MODEM_AT_RESPONSE_OK "OK"

/* Error-OK commands */
#define ERROR_OK_AT_CHUP "AT+CHUP"

/* Modem initialization commands */
#define AT_RESET "ATZ"
#define AT_ECHO_OFF "ATE0"
#define AT_NUMERIC_ERRORS_ON "AT+CMEE=1"
#define AT_CHARACTERS_SET_UCS2 "AT+CSCS=\"UCS2\""
#define AT_SMS_PDU_ON "AT+CMGF=0"
#define AT_STORAGE_ME "AT+CPMS=\"ME\",\"ME\",\"ME\""
#define AT_NEW_NOTIFICATIONS "AT+CNMI=2,1,0,0,0"
#define AT_DTR_HANGUP_OFF "AT+CVHU=0"
#define AT_CLIP_ON "AT+CLIP=1"
#define AT_COLP_ON "AT+COLP=1"
#define AT_EXTENDED_RING_ON "AT+CRC=1"
#define AT_CALL_WAITING_ON "AT+CCWA=1"
#define AT_SUPPLEMENTARY_SERVICES_ON "AT+CSSN=1,1"
#define AT_NETWORK_REGISTRATION_BASIC "AT+CREG=1"
#define AT_FULL_FUNCTIONALITY "AT+CFUN=1"

/* Modem initialization descriptions */
#define AT_RESET_DESC "Reset modem to defaults"
#define AT_ECHO_OFF_DESC "Disable command echo"
#define AT_NUMERIC_ERRORS_ON_DESC "Enable numeric error codes"
#define AT_CHARACTERS_SET_UCS2_DESC "Set character set to UCS2"
#define AT_SMS_PDU_ON_DESC "Set SMS to PDU mode"
#define AT_STORAGE_ME_DESC "Set storage to ME"
#define AT_NEW_NOTIFICATIONS_DESC "Set new message notifications"
#define AT_DTR_HANGUP_OFF_DESC "Disable DTR hangup"
#define AT_CLIP_ON_DESC "Enable calling line identification"
#define AT_COLP_ON_DESC "Enable connected line identification"
#define AT_EXTENDED_RING_ON_DESC "Enable extended format of incoming calls"
#define AT_CALL_WAITING_ON_DESC "Enable call waiting"
#define AT_SUPPLEMENTARY_SERVICES_ON_DESC                                      \
  "Enable supplementary service notifications"
#define AT_NETWORK_REGISTRATION_BASIC_DESC "Enable basic network registration"
#define AT_FULL_FUNCTIONALITY_DESC "Set full functionality"

/* Modem SMS related */
#define AT_SEND_SMS "AT+CMGS"
#define AT_SEND_SMS_LENGTH 7

/* Timing constants (milliseconds) */
#define MODEM_RESPONSE_DELAY_MS 500

/* Retry configuration */
#define MAX_INIT_RETRIES 3
#define INIT_RETRY_DELAY_SEC 3

// Function prototypes
int check_urgent_message(const char *buffer);
void categorize_and_output_line(const ModemTerminal *term, const char *line);
int init_modem(ModemTerminal *term);

#endif