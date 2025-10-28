#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <unistd.h>

/* 'Boolean' Constants */
#define FALSE 0
#define TRUE !FALSE

/* Char constants */
#define NEWLINE '\n'
#define NULL_TERMINATOR '\0'
#define CTRL_Z 26
#define CRLF "\r\n"
#define CRLF_LENGTH 2
#define AT_CRLF "AT\r\n"
#define AT_CRLF_LENGTH 4
#define SMS_END_MARKER '.'

/* Buffer sizes */
#define MAX_BUFFER 4096
#define MAX_COMMAND 512
#define MAX_STATUS_MSG 512
#define MAX_RESPONSE 256

#define MAX_STATUS_MSG 512
#define MAX_RESPONSE 256

/* Timing constants (milliseconds) */
#define MODEM_RESPONSE_DELAY_MS 500
#define COMMAND_DELAY_MS 100
#define SMS_SEND_DELAY_MS 500
#define THREAD_SLEEP_MS 10
#define MILLISECONDS_PER_SECOND 1000

/* Timing constants (microseconds) */
#define MICROSECONDS_PER_MILLISECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000

/* Retry configuration */
#define MAX_PORT_RETRIES 3
#define PORT_RETRY_DELAY_SEC 8
#define MAX_INIT_RETRIES 3
#define INIT_RETRY_DELAY_SEC 3

/* Termios settings */
#define SERIAL_VMIN 0
#define SERIAL_VTIME 10

/* Response message types */
#define MSG_TYPE_STATUS "STATUS"
#define MSG_TYPE_ERROR "ERROR"
#define MSG_TYPE_COMPLETE "COMPLETE"
#define MSG_TYPE_INFO "INFO"
#define MSG_TYPE_RESPONSE "RESPONSE"
#define MSG_TYPE_WARNING "WARNING"
#define MSG_TYPE_URGENT "URGENT"
#define MSG_TYPE_SMS "SMS"
#define MSG_TYPE_CALL "CALL"

#define EXIT_APP_COMMAND "exit"

/* Function prototypes */
ssize_t safe_write(int fd, const void *buf, size_t count);

void print_output(const char *type, const char *text);
void msleep(int milliseconds);

#endif