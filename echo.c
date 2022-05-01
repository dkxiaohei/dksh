#include <stdio.h>
#include <string.h>

int echo(int argc, char **args, int return_value)
{
    /* if called with "$?", print the return value of the last command */
    if (strcmp(args[1], "$?") == 0) {
        printf("%d\n", return_value);
        return 0;
    }

    while (--argc) {
        printf("%s", *++args);
        printf(argc > 0 ? " " : "");
    }
    putchar('\n');

    return 0;
}
