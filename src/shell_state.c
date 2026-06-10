#include "shell_state.h"
#include <errno.h>
#include <stddef.h>
#include <sys/wait.h>

int shell_state_init(shell_state* state)
{
    *state = (shell_state){0};

    state->permanent = arena_create(MiB(16), KiB(64));
    state->scratch = arena_create(MiB(4), KiB(64));
    if (!state->permanent || !state->scratch) {
        shell_state_destroy(state);
        return 0;
    }

    state->processes = PUSH_ARRAY(state->permanent, process_info, PROCESS_TABLE_CAPACITY);
    if (!state->processes) {
        shell_state_destroy(state);
        return 0;
    }

    state->process_count = 0;
    state->next_process_id = 1;
    state->should_exit = 0;
    return 1;
}

void shell_state_destroy(shell_state* state)
{
    if (state->scratch) {
        arena_destroy(state->scratch);
        state->scratch = NULL;
    }

    if (state->permanent) {
        arena_destroy(state->permanent);
        state->permanent = NULL;
    }
}

process_info* shell_add_process(shell_state* state, pid_t pid, const char* command)
{
    if (state->process_count >= PROCESS_TABLE_CAPACITY) {
        return NULL;
    }

    process_info* process = &state->processes[state->process_count++];
    process->id = state->next_process_id++;
    process->pid = pid;
    process->status = PROC_STATUS_RUNNING;
    process->exit_status = 0;
    process->command = arena_strdup(state->permanent, command);
    if (!process->command) {
        state->process_count--;
        return NULL;
    }

    return process;
}

process_info* shell_find_process(shell_state* state, int id)
{
    for (int i = 0; i < state->process_count; i++) {
        if (state->processes[i].id == id) {
            return &state->processes[i];
        }
    }

    return NULL;
}

void shell_process_apply_wait_status(process_info* process, int wait_status)
{
    if (WIFEXITED(wait_status)) {
        process->status = PROC_STATUS_DEAD;
        process->exit_status = WEXITSTATUS(wait_status);
    }
    else if (WIFSIGNALED(wait_status)) {
        process->status = PROC_STATUS_DEAD;
        process->exit_status = 128 + WTERMSIG(wait_status);
    }
    else if (WIFSTOPPED(wait_status)) {
        process->status = PROC_STATUS_STOPPED;
        process->exit_status = WSTOPSIG(wait_status);
    }
#ifdef WIFCONTINUED
    else if (WIFCONTINUED(wait_status)) {
        process->status = PROC_STATUS_RUNNING;
    }
#endif
}

void shell_refresh_processes(shell_state* state)
{
    for (int i = 0; i < state->process_count; i++) {
        process_info* process = &state->processes[i];
        if (process->status == PROC_STATUS_DEAD) {
            continue;
        }

        int wait_status = 0;
        pid_t result = waitpid(process->pid, &wait_status, WNOHANG | WUNTRACED | WCONTINUED);
        if (result == process->pid) {
            shell_process_apply_wait_status(process, wait_status);
        }
        else if (result == -1 && errno == ECHILD) {
            process->status = PROC_STATUS_DEAD;
        }
    }
}

const char* shell_process_status_name(int status)
{
    switch (status) {
    case PROC_STATUS_RUNNING:
        return "running";
    case PROC_STATUS_STOPPED:
        return "stopped";
    case PROC_STATUS_DEAD:
        return "dead";
    default:
        return "unknown";
    }
}
