#ifndef CALL_H
#define CALL_H

#include "../utils/utils.h"
#include "responses.h"

#define CALLER_ID_LENGTH 32

// Call state tracking
typedef struct {
  bool call_in_progress;
  char last_caller_id[CALLER_ID_LENGTH];
} CallState;

extern CallState call_state;

/* Function prototypes */
void init_call_state(void);
void handle_call_message(const char *line);

#endif