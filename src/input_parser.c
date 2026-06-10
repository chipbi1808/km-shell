#include "input_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

char* read_line(mem_arena* arena, FILE* input)
{
    char* line = NULL;
    size_t buf_size = 0;
    char* out;

    if (getline(&line, &buf_size, input) == -1) {
        free(line);
        if (feof(input)) {
            return NULL;
        }
        else {
            perror("shell: getline\n");
            exit(EXIT_FAILURE);
        }
    }

    out = arena_strdup(arena, line);
    free(line);
    if (!out) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    return out;
}

char** split_line(mem_arena* arena, char* line)
{
    int buf_size = TOKEN_BUFSIZE;
    int position = 0;
    char** tokens = PUSH_ARRAY(arena, char*, buf_size);
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
            char** new_tokens;
            buf_size += TOKEN_BUFSIZE;
            new_tokens = PUSH_ARRAY(arena, char*, buf_size);
            if (!new_tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
            memcpy(new_tokens, tokens, (u64)position * sizeof(char*));
            tokens = new_tokens;
        }

        token = strtok(NULL, TOKEN_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}
