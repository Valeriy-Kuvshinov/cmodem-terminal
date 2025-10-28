#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../headers/serial.h"
#include "../headers/utils.h"

/* Inner STATIC methods */
/* ==================================================================== */
static void log_open_attempt(const char *device, int attempt, int max_retries) {
  char status[MAX_STATUS_MSG];

  snprintf(status, sizeof(status), "Attempting to open port %s (attempt %d/%d)",
           device, attempt + 1, max_retries);
  print_output(MSG_TYPE_STATUS, status);
}

static void log_open_success(const char *device) {
  char status[MAX_STATUS_MSG];

  snprintf(status, sizeof(status), "Successfully opened port %s", device);
  print_output(MSG_TYPE_STATUS, status);
}

static void log_open_retry(const char *device, int retry_delay) {
  char msg[MAX_STATUS_MSG];

  snprintf(msg, sizeof(msg),
           "Port %s busy, retrying in %d seconds... (Error: %s)", device,
           retry_delay, strerror(errno));
  print_output(MSG_TYPE_STATUS, msg);
}

static void log_open_failure(const char *device, int max_retries) {
  char error[MAX_STATUS_MSG];

  snprintf(error, sizeof(error), "Failed to open port %s after %d attempts: %s",
           device, max_retries, strerror(errno));
  print_output(MSG_TYPE_ERROR, error);
}

static int configure_serial_port(int fd) {
  struct termios tty;

  if (tcgetattr(fd, &tty) < 0) {
    close(fd);

    return -1;
  }
  cfsetospeed(&tty, BAUDRATE);
  cfsetispeed(&tty, BAUDRATE);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_iflag &= ~IGNBRK;
  tty.c_lflag = 0;
  tty.c_oflag = 0;
  tty.c_cc[VMIN] = SERIAL_VMIN;
  tty.c_cc[VTIME] = SERIAL_VTIME;

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_cflag |= (CLOCAL | CREAD);
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) < 0) {
    close(fd);

    return -1;
  }
  return 0;
}

/* Outer methods */
/* ==================================================================== */
int open_serial_port(const char *device, int max_retries, int retry_delay) {
  int attempt, fd;

  for (attempt = 0; attempt < max_retries; attempt++) {
    log_open_attempt(device, attempt, max_retries);

    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);

    if (fd >= 0) {
      if (configure_serial_port(fd) == 0) {
        log_open_success(device);

        return fd;
      }
    }
    if (attempt < max_retries - 1) {
      log_open_retry(device, retry_delay);
      sleep(retry_delay);
    }
  }
  log_open_failure(device, max_retries);

  return -1;
}
