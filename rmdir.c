#include <stdio.h>
#include <unistd.h>

int my_rmdir(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }

    if (rmdir(args[1]) != 0) {
        perror("rmdir");
        return -1;
    }

    return 0;
}
