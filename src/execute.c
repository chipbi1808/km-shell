#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int args_count(char** args)
{
    int count = 0;
    while (args[count]) {
        count++;
    }
    return count;
}

static int remove_background_marker(char** args)
{
    int count = args_count(args);
    if (count > 0 && strcmp(args[count - 1], "&") == 0) {
        args[count - 1] = NULL;
        return 1;
    }

    return 0;
}

static char* command_text(shell_state* state, char** args)
{
    u64 len = 1;
    char* out;
    char* cursor;

    for (int i = 0; args[i]; i++) {
        len += (u64)strlen(args[i]) + 1;
    }

    out = PUSH_ARRAY_NZ(state->scratch, char, len);
    if (!out) {
        return NULL;
    }

    cursor = out;
    for (int i = 0; args[i]; i++) {
        u64 arg_len = (u64)strlen(args[i]);
        if (i > 0) {
            *cursor++ = ' ';
        }
        memcpy(cursor, args[i], arg_len);
        cursor += arg_len;
    }
    *cursor = '\0';

    return out;
}

int shell_launch(shell_state* state, char** args)
{
    pid_t pid;
    int wait_status;
    int background = remove_background_marker(args);
    char* command = command_text(state, args);

    if (!args[0]) {
        return 1;
    }

    if (!command) {
        fprintf(stderr, "shell: allocation error\n");
        return 1;
    }

    if (state->process_count >= PROCESS_TABLE_CAPACITY) {
        fprintf(stderr, "shell: process table is full\n");
        return 1;
    }

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("shell");
    }
    else {
        process_info* process = shell_add_process(state, pid, command);
        if (!process) {
            fprintf(stderr, "shell: process table is full\n");
            return 1;
        }

        if (background) {
            printf("[%d] %d\n", process->id, (int)process->pid);
            return 1;
        }

        do {
            if (waitpid(pid, &wait_status, WUNTRACED) == -1) {
                perror("shell");
                break;
            }
            shell_process_apply_wait_status(process, wait_status);
        } while (!WIFEXITED(wait_status) &&
                 !WIFSIGNALED(wait_status) &&
                 !WIFSTOPPED(wait_status));
    }

    return 1;
}

int shell_execute(shell_state* state, char** args)
{
    if (args == NULL || args[0] == NULL) {
        return 1;
    }

    if (is_builtin(args[0])) {
        return run_builtin(state, args);
    }

    return shell_launch(state, args);
}
