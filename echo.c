#include <stdio.h>
#include <string.h>

int echo(int argc, char **args, int return_value)
{
    /* if called with "$?", print return value of last command */
    if (strcmp(args[1], "$?") == 0)
        printf("%d\n", return_value);
    else {
        while (--argc) {
            printf("%s", *++args);
            printf(argc > 0 ? " " : "");
        }

        putchar('\n');
    }

    return 0;
}
