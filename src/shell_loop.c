#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "execute.h"
#include "input_parser.h"

static int shell_execute_line(shell_state* state, FILE* input)
{
    char* line;
    char** args;

    arena_clear(state->scratch);

    line = read_line(state->scratch, input);
    if (!line) {
        return 0;
    }

    args = split_line(state->scratch, line);
    shell_execute(state, args);
    shell_refresh_processes(state);
    return 1;
}

void shell_loop(void)
{
    shell_state state = {0};

    if (!shell_state_init(&state)) {
        fprintf(stderr, "shell: failed to initialize shell state\n");
        exit(EXIT_FAILURE);
    }

    while (!state.should_exit) {
        printf("> ");
        fflush(stdout);

        if (!shell_execute_line(&state, stdin)) {
            break;
        }
    }

    shell_state_destroy(&state);
}

int shell_run_file(shell_state* state, const char* path)
{
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("shell");
        return 1;
    }

    while (!state->should_exit && shell_execute_line(state, file)) {
    }

    fclose(file);
    return !state->should_exit;
}
