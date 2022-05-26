#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int echo(int argc, char **args, int return_value)
{
    char *env_var_value;

    /* if called with "$?", print the return value of the last command */
    if (args[1] && strcmp(args[1], "$?") == 0) {
        printf("%d\n", return_value);
        return 0;
    }

    if (args[1] && args[1][0] == '$') {
        env_var_value = getenv(&args[1][1]);    /* omit the preceding '$' */
        printf("%s\n", env_var_value ? env_var_value : "");
        return 0;
    }

    while (--argc) {
        printf("%s", *++args);
        printf(argc > 0 ? " " : "");
    }
    putchar('\n');

    return 0;
}
