#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE 1
#define BUFSIZE 512

int cat(int argc, char **args)
{
    char buf[BUFSIZE];
    int fd, ret;
    int return_error = FALSE;

    if (argc == 1) {    // if no files specified, then use STDIN
        fd = 0;

        while ((ret = read(fd, buf, sizeof(buf))) > 0)
            if (write(1, buf, ret) < 0) {
                perror("write");
                return -1;
            }
    }
    else {
        while (argc-- > 1) {    // support multiple files
            fd = open(*++args, O_RDONLY);

            if (fd == -1) {
                fprintf(stderr,
                        "cat: %s: No such file or directory\n", *args);
                return_error = TRUE;
                continue;    // try the next file (if any)
            }

            while ((ret = read(fd, buf, sizeof(buf))) > 0)
                if (write(1, buf, ret) < 0) {    // write to STDOUT
                    perror("write");
                    return -1;
                }

            if (close(fd) == -1) {
                perror("close");
                return -1;
            }
        }
    }

    return return_error ? -1 : 0;
}
