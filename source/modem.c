#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "../headers/modem.h"
#include "../headers/utils.h"

static const char *urgent_msg_arr[] = {URGENT_MSG_CMTI,
                                       URGENT_MSG_CMT,
                                       URGENT_MSG_RING,
                                       URGENT_MSG_CRING,
                                       URGENT_MSG_CLIP,
                                       URGENT_MSG_COLP,
                                       URGENT_MSG_VOICE_CALL_BEGIN,
                                       URGENT_MSG_VOICE_CALL_END,
                                       URGENT_MSG_NO_CARRIER,
                                       URGENT_MSG_BUSY,
                                       URGENT_MSG_NO_ANSWER,
                                       URGENT_MSG_CME_ERROR,
                                       URGENT_MSG_CMS_ERROR,
                                       URGENT_MSG_SIMCARD_NOT_AVAILABLE,
                                       NULL};

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
static void handle_sms_message(const char *line) {
  print_output(MSG_TYPE_SMS, line);
}

static void handle_call_end_message(const char *line) {
  char msg[MAX_BUFFER];

  snprintf(msg, sizeof(msg), "CALL_END: %s", line);
  print_output(MSG_TYPE_COMPLETE, msg);
}

static void handle_call_begin_message(const char *line) {
  char msg[MAX_BUFFER];

  snprintf(msg, sizeof(msg), "CALL_CONNECTED: %s", line);
  print_output(MSG_TYPE_COMPLETE, msg);
}

static void handle_call_message(const char *line) {
  print_output(MSG_TYPE_CALL, line);

  if (IS_CALL_END_MESSAGE(line)) {
    handle_call_end_message(line);
  } else if (IS_CALL_BEGIN_MESSAGE(line)) {
    handle_call_begin_message(line);
  }
}

static void handle_error_message(const char *line) {
  print_output(MSG_TYPE_ERROR, line);
}

static void log_init_failure_final(const char *desc, const char *response) {
  char status[MAX_STATUS_MSG];

  snprintf(status, sizeof(status), "%s failed after %d attempts (Response: %s)",
           desc, MAX_INIT_RETRIES,
           strlen(response) > 0 ? response : "No response");
  print_output(MSG_TYPE_WARNING, status);
}

static int send_command(ModemTerminal *term, const char *cmd, char *response,
                        size_t response_size) {
  pthread_mutex_lock(&term->serial_mutex);
  safe_write(term->fd, cmd, strlen(cmd));
  safe_write(term->fd, CRLF, CRLF_LENGTH);
  pthread_mutex_unlock(&term->serial_mutex);

  msleep(MODEM_RESPONSE_DELAY_MS);

  int n = read(term->fd, response, response_size - 1);

  if (n > 0) {
    response[n] = NULL_TERMINATOR;
  }
  return n;
}

static void log_init_status(const char *desc, int attempt) {
  char status[MAX_STATUS_MSG];

  if (attempt > 0) {
    snprintf(status, sizeof(status), "Initializing: %s (retry %d/%d)", desc,
             attempt + 1, MAX_INIT_RETRIES);
  } else {
    snprintf(status, sizeof(status), "Initializing: %s", desc);
  }
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

  snprintf(status, sizeof(status),
           "%s failed, retrying in %d seconds... (Response: %s)", desc,
           INIT_RETRY_DELAY_SEC,
           strlen(response) > 0 ? response : "No response");
  print_output(MSG_TYPE_WARNING, status);
}

/* Outer methods */
/* ==================================================================== */
int check_urgent_message(const char *buffer) {
  int i;

  for (i = 0; urgent_msg_arr[i] != NULL; i++) {
    if (strstr(buffer, urgent_msg_arr[i]) != NULL) {
      return 1;
    }
  }
  return 0;
}

void categorize_and_output_line(ModemTerminal *term, const char *line) {
  if (strlen(line) == 0)
    return;

  if (IS_SMS_MESSAGE(line))
    handle_sms_message(line);

  else if (IS_CALL_MESSAGE(line))
    handle_call_message(line);

  else if (IS_ERROR_MESSAGE(line))
    handle_error_message(line);

  // Success
  else if (strcmp(line, MODEM_AT_RESPONSE_OK) == 0)
    print_output(MSG_TYPE_COMPLETE, MODEM_AT_RESPONSE_OK);

  // Generic ERROR
  else if (strstr(line, MSG_TYPE_ERROR)) {
    if (IS_ERROR_OK_COMMAND(term->last_command))
      print_output(MSG_TYPE_COMPLETE, MODEM_AT_RESPONSE_OK);
    else
      print_output(MSG_TYPE_ERROR, line);
  }
  // SIM card errors
  else if (IS_SIM_ERROR_MESSAGE(line))
    print_output(MSG_TYPE_ERROR, "SIMCARD MISSING");

  // Default
  else
    print_output(MSG_TYPE_RESPONSE, line);
}

int init_modem(ModemTerminal *term) {
  int i;

  for (i = 0; init_commands[i][0] != NULL; i++) {
    const char *cmd = init_commands[i][0];
    const char *desc = init_commands[i][1];

    int attempt;
    int success = 0;

    for (attempt = 0; attempt < MAX_INIT_RETRIES; attempt++) {
      log_init_status(desc, attempt);

      char response[MAX_BUFFER] = {0};
      int n = send_command(term, cmd, response, sizeof(response));

      if (n > 0) {
        if (strstr(response, MODEM_AT_RESPONSE_OK)) {
          success = 1;

          break;
        } else if (strstr(response, MSG_TYPE_ERROR) || n == 0) {
          if (attempt < MAX_INIT_RETRIES - 1) {
            log_init_failure_retry(desc, attempt, response);
            sleep(INIT_RETRY_DELAY_SEC);
          } else
            log_init_failure_final(desc, response);
        } else {
          log_init_success(desc, response);

          success = 1;

          break;
        }
      }
    }
    if (success)
      msleep(MODEM_RESPONSE_DELAY_MS);
  }
  return 1;
}