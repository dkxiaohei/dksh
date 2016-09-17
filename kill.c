#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int my_kill(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage: %s -<sig (default as TERM, 15)> <pid>\n", args[0]);
        return -1;
    }

    pid_t pid;
    int sig = 15;    // default, TERM
    ++args;    // skip the command

    if ((*args)[0] == '-') {
        sig = atoi(&(*args)[1]);
        ++args;    // skip the sig
    }

    pid = atoi(*args);
    if (kill(pid, sig) == -1) {
        perror("kill");
        return -1;
    }

    return 0;
}
