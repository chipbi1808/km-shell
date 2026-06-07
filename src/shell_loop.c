#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "execute.h"
#include "input_parser.h"

void shell_loop(void)
{
    char* line;
    char** args;
    int status;
    do {
        printf("> ");
        fflush(stdout);
        line = read_line();
        args = split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);

    } while (status);
}
