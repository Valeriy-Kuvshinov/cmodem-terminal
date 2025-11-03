#include "../../include/globals/globals.h"

_Thread_local const char *thread_name = NULL;

_Alignas(32) _Atomic bool exit_requested = false;
