#include "builtin.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int shell_cd(char** args);
static int shell_help(char** args);
static int shell_exit(char** args);

static char* builtin_str[] = {
    "cd",
    "help",
    "exit",
};

static int (*builtin_func[])(char**) = {
    &shell_cd,
    &shell_help,
    &shell_exit,
};

static int num_builtins(void)
{
    return sizeof(builtin_str) / sizeof(char*);
}

static int shell_cd(char** args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    }
    else if (chdir(args[1]) != 0) {
        perror("shell");
    }

    return 1;
}

static int shell_help(char** args)
{
    (void)args;

    printf("Tiny Shell\n");
    printf("Type program names and arguments, then press enter.\n");
    printf("The following commands are built in:\n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

static int shell_exit(char** args)
{
    (void)args;
    return 0;
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

int run_builtin(char** args)
{
    if (args == NULL || args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return 1;
}
