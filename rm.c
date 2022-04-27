#include <stdio.h>
#include <unistd.h>

int rm(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", args[0]);
        return -1;
    }

    int ret = unlink(args[1]);
    if (ret) {
        perror("unlink");
        return -1;
    }

    return 0;
}
