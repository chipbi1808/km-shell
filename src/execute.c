#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int shell_launch(char** args)
{
    pid_t pid;
    int status;

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
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int shell_execute(char** args)
{
    if (args == NULL || args[0] == NULL) {
        return 1;
    }

    if (is_builtin(args[0])) {
        return run_builtin(args);
    }

    return shell_launch(args);
}
