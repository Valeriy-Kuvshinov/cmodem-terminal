#ifndef SERIAL_H
#define SERIAL_H

#include "../utils/utils.h"

// Serial port constants
#define CRTSCTS 0x80000000

#define BAUDRATE B230400

/* Termios settings */
#define SERIAL_VMIN 0
#define SERIAL_VTIME 10

// Function prototype for opening serial port
int open_serial_port(const char *device, int max_retries, int retry_delay);

#endif