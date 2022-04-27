#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dksh.h"

int tee(int argc, char **args)
{
    char buf[BUFSIZE];
    int i, ret, flags;
    int append = FALSE;

    ++args;    /* skip the command itself */
    if (argc > 2 && strcmp(*args, "-a") == 0) {
        append = TRUE;
        --argc;    /* argc is used to determine the number of files */
        ++args;    /* skip the option (-a) */
    }
    flags = append ? O_WRONLY|O_CREAT|O_APPEND : O_WRONLY|O_CREAT;

    /* the number of files is argc, including STDIN */
    int *fd = calloc(argc, sizeof(int));
    if (argc > 1)
        /* fd[0] has already been set to 0 (STDIN) by calloc */
        for (i = 1; i < argc; i++, args++) {
            fd[i] = open(*args, flags, 0664);
            if (!fd[i]) {
                fprintf(stderr, "tee: %s: No such file or directory\n", *args);
                free(fd);
                return -1;
            }
        }

    /* read from STDIN */
    while ((ret = read(0, buf, sizeof(buf))) > 0) {
        for (i = 0; i < argc; i++)
            if (write(fd[i], buf, ret) < 0) {
                perror("write");
                free(fd);
                return -1;
            }
        memset(buf, 0, sizeof(buf));
    }

    if (argc > 1)
        for (i = 1; i < argc; i++)    /* excluding STDIN (0) */
            if (close(fd[i]) == -1) {
                perror("close");
                free(fd);
                return -1;
            }

    free(fd);

    return 0;
}
