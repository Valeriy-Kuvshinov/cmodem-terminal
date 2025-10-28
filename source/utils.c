#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include "../headers/utils.h"

void print_output(const char *type, const char *text) {
  printf("[%s] %s%c", type, text, NEWLINE);
  fflush(stdout);
}

void msleep(int ms) {
  struct timeval tv;

  tv.tv_sec = ms / MILLISECONDS_PER_SECOND;
  tv.tv_usec = (ms % MILLISECONDS_PER_SECOND) * MICROSECONDS_PER_MILLISECOND;

  select(0, NULL, NULL, NULL, &tv);
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
  ssize_t ret = write(fd, buf, count);

  if (ret < 0) {
    print_output("ERROR", "Write to serial port failed");
  }
  return ret;
}