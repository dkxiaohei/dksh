#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

int my_chmod(int argc, char **args)
{
    mode_t mode;

    if (argc < 3 || !isdigit(args[1][0])) {
        fprintf(stderr, "Usage: %s <mode (such as 664)> <file>\n", args[0]);
        return -1;
    }

    mode = ((args[1][0]-'0')<<6) + ((args[1][1]-'0')<<3) + (args[1][2]-'0');

    if (chmod(args[2], mode) != 0) {
        perror("chmod");
        return -1;
    }

    return 0;
}
