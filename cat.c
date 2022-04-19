#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define BUFSIZE 512

int simple_output(char buf[], int size)
{
    // write to STDOUT
    if (write(1, buf, size) < 0) {
        perror("write");
        return -1;
    }
    return 0;
}

int cat(int argc, char **args)
{
    char buf[BUFSIZE];
    int fd, ret;
    FILE *fp;
    int print_line_number = FALSE, line_number;
    int return_error = FALSE;

    // if no files specified, then use STDIN
    if (argc == 1) {
        fd = 0;
        while ((ret = read(fd, buf, sizeof(buf))) > 0)
            if (simple_output(buf, ret) != 0)
                return -1;
    } else {
        while (argc-- > 1) {    // support multiple files
            if (strcmp(*++args, "-n") == 0) {
                print_line_number = TRUE;
                continue;
            } else
                --args;
            line_number = 0;

            fp = fopen(*++args, "r");
            if (fp == NULL) {
                fprintf(stderr, "cat: %s: No such file or directory\n", *args);
                return_error = TRUE;
                continue;    // try the next file (if any)
            }

            while ((fgets(buf, sizeof(buf), fp)) != NULL) {
                if (print_line_number)
                    printf("%6d\t", ++line_number);
                if ((fputs(buf, stdout)) == EOF) {
                    perror("fputs");
                    return -1;
                }
            }

            if (fclose(fp) == EOF) {
                perror("fclose");
                return -1;
            }
        }
    }

    return return_error ? -1 : 0;
}
