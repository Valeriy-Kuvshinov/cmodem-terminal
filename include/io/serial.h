#ifndef SERIAL_H
#define SERIAL_H

#include <fcntl.h>
#include <termios.h>

#include "../config.h"
#include "../globals/buffers.h"
#include "../modem/responses.h"
#include "../utils/utils.h"

#define MAX_PORT_RETRIES 3

/* Termios custom settings */
#define SERIAL_VMIN 0
#define SERIAL_VTIME 10

int open_serial_port(const char *device);

#endif