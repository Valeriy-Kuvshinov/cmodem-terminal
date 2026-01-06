#ifndef COMMANDS_H
#define COMMANDS_H

#define AT_TEST "AT"
#define AT_TEST_DESC "Test modem connection"

/* Modem SMS related */
#define AT_SEND_SMS "AT+CMGS"
#define AT_SEND_SMS_LENGTH 7

/* Macros */
/* Error-OK commands */
#define IS_ERROR_OK_COMMAND(cmd) (strstr(cmd, "AT+CHUP"))

#endif