#include <stdio.h>
#include <sys/stat.h>

int my_mkdir(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }

    int ret = mkdir(args[1], 0775);
    if (ret) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
