#include <string.h>

#include "../../include/core/modem.h"
#include "../../include/utils/utils.h"

static const char *init_commands[][2] = {
    {AT_RESET, AT_RESET_DESC},
    {AT_ECHO_OFF, AT_ECHO_OFF_DESC},
    {AT_NUMERIC_ERRORS_ON, AT_NUMERIC_ERRORS_ON_DESC},
    {AT_CHARACTERS_SET_UCS2, AT_CHARACTERS_SET_UCS2_DESC},
    {AT_SMS_PDU_ON, AT_SMS_PDU_ON_DESC},
    {AT_STORAGE_ME, AT_STORAGE_ME_DESC},
    {AT_NEW_NOTIFICATIONS, AT_NEW_NOTIFICATIONS_DESC},
    {AT_DTR_HANGUP_OFF, AT_DTR_HANGUP_OFF_DESC},
    {AT_CLIP_ON, AT_CLIP_ON_DESC},
    {AT_COLP_ON, AT_COLP_ON_DESC},
    {AT_EXTENDED_RING_ON, AT_EXTENDED_RING_ON_DESC},
    {AT_CALL_WAITING_ON, AT_CALL_WAITING_ON_DESC},
    {AT_SUPPLEMENTARY_SERVICES_ON, AT_SUPPLEMENTARY_SERVICES_ON_DESC},
    {AT_NETWORK_REGISTRATION_BASIC, AT_NETWORK_REGISTRATION_BASIC_DESC},
    {AT_FULL_FUNCTIONALITY, AT_FULL_FUNCTIONALITY_DESC},
    {NULL, NULL}};

/* Inner STATIC methods */
/* ==================================================================== */
static void log_init_failure_final(const char *desc, const char *response) {
  char status[MAX_STATUS_MSG];
  size_t length = strlen(response);
  const char *response_msg = length > 0 ? response : "No response";

  snprintf(status, sizeof(status), "%s failed after %d attempts (Response: %s)",
           desc, MAX_INIT_RETRIES, response_msg);
  print_output(MSG_TYPE_WARNING, status);
}

static int send_command(ModemTerminal *term, const char *cmd, char *response,
                        size_t response_size) {
  int n;

  pthread_mutex_lock(&term->serial_mutex);

  safe_write(term->fd, cmd, strlen(cmd));
  safe_write(term->fd, CRLF, CRLF_LENGTH);

  pthread_mutex_unlock(&term->serial_mutex);

  msleep(MODEM_RESPONSE_DELAY_MS);

  n = read(term->fd, response, response_size - 1);

  if (n > 0)
    response[n] = NULL_TERMINATOR;

  return n;
}

static void log_init_status(const char *desc, int attempt) {
  char status[MAX_STATUS_MSG];

  if (attempt > 0)
    snprintf(status, sizeof(status), "Initializing: %s (retry %d/%d)", desc,
             attempt + 1, MAX_INIT_RETRIES);
  else
    snprintf(status, sizeof(status), "Initializing: %s", desc);

  print_output(MSG_TYPE_STATUS, status);
}

static void log_init_success(const char *desc, const char *response) {
  char status[MAX_STATUS_MSG];

  snprintf(status, sizeof(status), "%s returned: %s", desc, response);
  print_output(MSG_TYPE_INFO, status);
}

static void log_init_failure_retry(const char *desc, int attempt,
                                   const char *response) {
  char status[MAX_STATUS_MSG];
  size_t length = strlen(response);
  const char *response_msg = length > 0 ? response : "No response";

  snprintf(status, sizeof(status),
           "%s failed, retrying in %d seconds... (Response: %s)", desc,
           INIT_RETRY_DELAY_SEC, response_msg);
  print_output(MSG_TYPE_WARNING, status);
}

static int process_response(const char *response, int n, int attempt,
                            const char *desc) {
  if (strstr(response, MODEM_AT_RESPONSE_OK))
    return TRUE;
  else if (strstr(response, MSG_TYPE_ERROR) || n == 0) {
    if (attempt < MAX_INIT_RETRIES - 1) {
      log_init_failure_retry(desc, attempt, response);

      sleep(INIT_RETRY_DELAY_SEC);
    } else
      log_init_failure_final(desc, response);

    return FALSE;
  } else {
    log_init_success(desc, response);

    return TRUE;
  }
}

static int attempt_init(ModemTerminal *term, const char *cmd,
                        const char *desc) {
  int attempt;
  int success = FALSE;

  for (attempt = 0; attempt < MAX_INIT_RETRIES; attempt++) {
    char response[MAX_BUFFER] = {0};
    int n;

    log_init_status(desc, attempt);

    n = send_command(term, cmd, response, sizeof(response));

    if (n > 0 && process_response(response, n, attempt, desc)) {
      success = TRUE;

      break;
    }
  }
  return success;
}

/* Outer methods */
/* ==================================================================== */
int init_modem(ModemTerminal *term) {
  int i;

  for (i = 0; init_commands[i][0] != NULL; i++) {
    const char *cmd = init_commands[i][0];
    const char *desc = init_commands[i][1];

    if (attempt_init(term, cmd, desc))
      msleep(MODEM_RESPONSE_DELAY_MS);
  }
  return 1;
}