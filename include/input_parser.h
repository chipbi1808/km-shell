#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdio.h>
#include "arena.h"

char* read_line(mem_arena* arena, FILE* input);
char** split_line(mem_arena* arena, char* line);

#endif
