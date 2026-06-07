#include <stdlib.h>
#include "shell_loop.h"


int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    shell_loop();

    return EXIT_SUCCESS;
}
