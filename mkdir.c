#include <stdio.h>
#include <sys/stat.h>

#define DIRPERM 0755

int my_mkdir(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory>\n", args[0]);
        return -1;
    }

    if (mkdir(args[1], DIRPERM) != 0) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
