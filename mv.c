#include <stdio.h>

int mv(int argc, char **args)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <oldpath> <newpath>\n",
                args[0]);
        return -1;
    }

    int ret;
    ret = rename(args[1], args[2]);
    if (ret) {
        perror("rename");
        return -1;
    }

    return 0;
}
