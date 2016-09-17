#include <stdio.h>
#include <unistd.h>

int my_rmdir(int argc, char **args)
{
    int ret;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }

    ret = rmdir(args[1]);
    if (ret) {
        perror("rmdir");
        return -1;
    }

    return 0;
}
