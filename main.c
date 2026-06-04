#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Shell loop
//Input parsing
//Command execution
//Handle built-in command exp. cd, pwd, echo, env, setenv, unsetevn, which, exit
//Execute external-command
//Manage environment variables
//Manage path
//Error handling

void shell_loop(char** env) {
    char* input = NULL;
    size_t input_size = 0;

    char** args;
    while (1) {
        printf("[minh_shell]> ");
        if (getline(&input, &input_size, stdin) == -1) {
            perror("readline");
            break;
        }

        input[strcspn(input, "\n")] = 0;
        printf("Input: %s\n", input);
    }
    free(input);
}


int main(int argc, char** argv, char** env) {
    (void) argc;
    (void) argv;
    shell_loop(env);

    return 0;
}
