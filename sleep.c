#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int my_sleep(int argc, char **args)
{
    int seconds;

    if (argc < 2) {
        fprintf(stderr, "sleep: missing operand\n");
        return 1;
    }

    seconds = atoi(args[1]);
    return sleep(seconds);
}
