#ifndef SERIAL_H
#define SERIAL_H

#include "../utils/utils.h"

/* File control flags */
#define O_RDWR 02
#define O_NOCTTY 0400
#define O_SYNC 04010000

/* Termios control characters */
#define VMIN 6
#define VTIME 5

/* Termios flags */
#define CSIZE 0000060
#define CS8 0000060
#define IGNBRK 0000001
#define IXON 0002000
#define IXOFF 0010000
#define IXANY 0004000
#define CLOCAL 0004000
#define CREAD 0000200
#define PARENB 0000400
#define PARODD 0001000
#define CSTOPB 0000100
#define CRTSCTS 0x80000000
#define TCSANOW 0

/* Baud rates */
#define BAUDRATE 0010003

/* Termios settings */
#define SERIAL_VMIN 0
#define SERIAL_VTIME 10

int open_serial_port(const char *device, int max_retries, int retry_delay);

#endif