#ifndef SHELL_STATE_H
#define SHELL_STATE_H

#include <sys/types.h>
#include "arena.h"

#define PROC_STATUS_DEAD 0
#define PROC_STATUS_RUNNING 1
#define PROC_STATUS_STOPPED 2

#define PROCESS_TABLE_CAPACITY 128

typedef struct {
    int id;
    pid_t pid;
    int status;
    int exit_status;
    char* command;
} process_info;

typedef struct {
    mem_arena* permanent;
    mem_arena* scratch;
    process_info* processes;
    int process_count;
    int next_process_id;
    int should_exit;
} shell_state;

int shell_state_init(shell_state* state);
void shell_state_destroy(shell_state* state);
process_info* shell_add_process(shell_state* state, pid_t pid, const char* command);
process_info* shell_find_process(shell_state* state, int id);
void shell_refresh_processes(shell_state* state);
void shell_process_apply_wait_status(process_info* process, int wait_status);
const char* shell_process_status_name(int status);

#endif
