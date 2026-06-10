#ifndef EXECUTE_H
#define EXECUTE_H

#include "shell_state.h"

int shell_execute(shell_state* state, char** args);
int shell_launch(shell_state* state, char** args);

#endif
