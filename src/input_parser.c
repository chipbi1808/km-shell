#include "input_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

char* read_line(void)
{
    char* line = NULL;
    size_t buf_size = 0;
    if (getline(&line, &buf_size, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        }
        else {
            perror("shell: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

char** split_line(char* line)
{
    int buf_size = TOKEN_BUFSIZE;
    int position = 0;
    char** tokens = malloc(buf_size * sizeof(char*));
    char** tokens_backup;
    char* token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buf_size) {
            buf_size += TOKEN_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buf_size * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOKEN_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}
