#ifndef COMMANDS_H
#define COMMANDS_H

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
#define AT_CLIP_ON "AT+CLIP=1"
#define AT_EXTENDED_RING_ON "AT+CRC=1"
#define AT_NETWORK_REGISTRATION_BASIC "AT+CREG=1"

/* Modem initialization descriptions */
#define AT_RESET_DESC "Reset modem to defaults"
#define AT_ECHO_OFF_DESC "Disable command echo"
#define AT_NUMERIC_ERRORS_ON_DESC "Enable numeric error codes"
#define AT_CHARACTERS_SET_UCS2_DESC "Set character set to UCS2"
#define AT_SMS_PDU_ON_DESC "Set SMS to PDU mode"
#define AT_STORAGE_ME_DESC "Set storage to ME"
#define AT_NEW_NOTIFICATIONS_DESC "Set new message notifications"
#define AT_CLIP_ON_DESC "Enable calling line identification"
#define AT_EXTENDED_RING_ON_DESC "Enable extended format of incoming calls"
#define AT_NETWORK_REGISTRATION_BASIC_DESC "Enable basic network registration"

/* Modem SMS related */
#define AT_SEND_SMS "AT+CMGS"
#define AT_SEND_SMS_LENGTH 7

/* Macros */
#define IS_ERROR_OK_COMMAND(cmd) (strstr(cmd, ERROR_OK_AT_CHUP))

#endif