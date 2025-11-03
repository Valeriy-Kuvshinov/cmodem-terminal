#include "../../include/utils/utils.h"

/* Thread-safe logging with thread identification */
void print_output(const char *type, const char *text) {
  if (thread_name)
    printf("[%s] %s: %s%c", thread_name, type, text, NEWLINE);
  else
    printf("%s: %s\n", type, text);

  fflush(stdout);
}

void msleep(int ms) {
  struct timeval timeval;

  timeval.tv_sec = ms / MILLISECONDS_PER_SECOND;
  timeval.tv_usec =
      (ms % MILLISECONDS_PER_SECOND) * MICROSECONDS_PER_MILLISECOND;

  select(0, NULL, NULL, NULL, &timeval);
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
  ssize_t ret = write(fd, buf, count);

  if (ret < 0)
    print_output("ERROR", strerror(errno));

  return ret;
}