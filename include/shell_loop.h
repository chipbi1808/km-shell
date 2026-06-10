#ifndef SHELL_LOOP_H
#define SHELL_LOOP_H

#include "shell_state.h"

void shell_loop(void);
int shell_run_file(shell_state* state, const char* path);

#endif
