#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>

int my_kill(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage: %s -<signum (default as 15)> <pid>\n",
                args[0]);
        return -1;
    }

    pid_t pid;
    int sig = 15;    /* default, SIGTERM */
    ++args;    /* skip the command */

    if ((*args)[0] == '-') {
        if (!isdigit((*args)[1])) {
            fprintf(stderr, "Error: signum must be a number\n");
            return -1;
        }
        sig = atoi(&(*args)[1]);
        ++args;    /* skip the sig */
    }

    pid = atoi(*args);
    if (kill(pid, sig) == -1) {
        perror("kill");
        return -1;
    }

    return 0;
}
