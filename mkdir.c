#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int my_mkdir(int argc, char **args)
{
    int ret;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }

    ret = mkdir(args[1], 0775);
    if (ret) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
