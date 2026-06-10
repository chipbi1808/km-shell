#ifndef BUILTIN_H
#define BUILTIN_H

#include "shell_state.h"

int is_builtin(char* command);
int run_builtin(shell_state* state, char** args);

#endif
