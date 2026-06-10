#include "builtin.h"
#include "shell_loop.h"
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int shell_cd(shell_state* state, char** args);
static int shell_help(shell_state* state, char** args);
static int shell_exit(shell_state* state, char** args);
static int shell_plist(shell_state* state, char** args);
static int shell_stop(shell_state* state, char** args);
static int shell_run(shell_state* state, char** args);
static int shell_runfile(shell_state* state, char** args);

typedef int (*builtin_fn)(shell_state* state, char** args);

static char* builtin_str[] = {
    "cd",
    "help",
    "exit",
    "plist",
    "jobs",
    "stop",
    "run",
    "runfile",
    "source",
};

static builtin_fn builtin_func[] = {
    &shell_cd,
    &shell_help,
    &shell_exit,
    &shell_plist,
    &shell_plist,
    &shell_stop,
    &shell_run,
    &shell_runfile,
    &shell_runfile,
};

static int num_builtins(void)
{
    return sizeof(builtin_str) / sizeof(char*);
}

static int parse_process_id(char* arg, int* out)
{
    char* end = NULL;
    long id;

    if (!arg) {
        return 0;
    }

    id = strtol(arg, &end, 10);
    if (*arg == '\0' || *end != '\0' || id <= 0 || id > INT_MAX) {
        return 0;
    }

    *out = (int)id;
    return 1;
}

static int shell_cd(shell_state* state, char** args)
{
    (void)state;

    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    }
    else if (chdir(args[1]) != 0) {
        perror("shell");
    }

    return 1;
}

static int shell_help(shell_state* state, char** args)
{
    (void)state;
    (void)args;

    printf("Tiny Shell\n");
    printf("Type program names and arguments, then press enter.\n");
    printf("Use '&' at the end of a command to run it in the background.\n");
    printf("The following commands are built in:\n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Process commands: plist/jobs, stop <id>, run <id>, runfile <file>\n");
    return 1;
}

static int shell_exit(shell_state* state, char** args)
{
    (void)args;
    state->should_exit = 1;
    return 0;
}

static int shell_plist(shell_state* state, char** args)
{
    (void)args;
    shell_refresh_processes(state);

    printf("%-4s %-8s %-9s %s\n", "ID", "PID", "STATUS", "COMMAND");
    for (int i = 0; i < state->process_count; i++) {
        process_info* process = &state->processes[i];
        printf("%-4d %-8d %-9s %s\n",
               process->id,
               (int)process->pid,
               shell_process_status_name(process->status),
               process->command);
    }

    return 1;
}

static int shell_stop(shell_state* state, char** args)
{
    int id;
    process_info* process;

    if (!parse_process_id(args[1], &id)) {
        fprintf(stderr, "shell: usage: stop <process-id>\n");
        return 1;
    }

    process = shell_find_process(state, id);
    if (!process) {
        fprintf(stderr, "shell: process %d not found\n", id);
        return 1;
    }

    if (kill(process->pid, SIGSTOP) != 0) {
        perror("shell");
        return 1;
    }

    process->status = PROC_STATUS_STOPPED;
    return 1;
}

static int shell_run(shell_state* state, char** args)
{
    int id;
    process_info* process;

    if (!parse_process_id(args[1], &id)) {
        fprintf(stderr, "shell: usage: run <process-id>\n");
        return 1;
    }

    process = shell_find_process(state, id);
    if (!process) {
        fprintf(stderr, "shell: process %d not found\n", id);
        return 1;
    }

    if (kill(process->pid, SIGCONT) != 0) {
        perror("shell");
        return 1;
    }

    process->status = PROC_STATUS_RUNNING;
    return 1;
}

static int shell_runfile(shell_state* state, char** args)
{
    if (!args[1]) {
        fprintf(stderr, "shell: usage: runfile <file>\n");
        return 1;
    }

    return shell_run_file(state, args[1]);
}

int is_builtin(char* command)
{
    if (command == NULL) {
        return 0;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(command, builtin_str[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int run_builtin(shell_state* state, char** args)
{
    if (args == NULL || args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(state, args);
        }
    }

    return 1;
}
