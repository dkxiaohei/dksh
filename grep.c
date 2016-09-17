/* Inspired by K&R C - "The C Progamming Language" */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 512

int grep(int argc, char **args)
{
    // calloc will initialize the memory to zero
    char *buf = calloc(BUFSIZE, sizeof(char));
    long lineno = 0;
    int c, except = 0, number = 0, found = 0;
    // used as the 2nd parameter of getline
    size_t tmp = BUFSIZE;
    FILE *fs;

    // parse the options
    while(--argc > 0 && (*++args)[0] == '-') {
        // prevent the value of args[0] (which is a pointer)
        // from being changed
        char *save_args_0 = args[0];

        while((c = *++save_args_0))
            switch(c)
            {
                // invert the sense of matching, to select non-matching lines
                case 'v':  except = 1;  break;
                // prefix each line of output with the 1-based line number
                // within its input file
                case 'n':  number = 1;  break;
                default:  printf("grep: illegal option %c\n", c);
                          argc = 0;
                          found = -1;
                          break;
            }
    }

    // argc and args have been changed through the while body
    if(argc != 1 && argc != 2) {
        printf("Usage: grep -v -n pattern [file]\n");
        return -1;
    }
    else if (argc == 1)    // if no file specified, then use STDIN
        fs = stdin;
    else {    // argc == 2
        fs = fopen(*(args + 1), "r");
        if (!fs) {
            fprintf(stderr, "grep: %s: No such file or directory\n", *(args + 1));
            return -1;
        }
    }

    // <stdio.h>: ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    while (getline(&buf, &tmp, fs) > 0) {
        lineno++;
        // kind of tricky
        if((strstr(buf, *args) != NULL) != except)
        {
            if(number)
                printf("%ld:", lineno);
            printf("%s", buf);
            found++;
        }
        // clear buf before reading the next line
        memset(buf, 0, BUFSIZE);
    }

    if (buf)
        free(buf);

    if (fs != stdin)
        if (fclose(fs) == EOF) {
            perror("close");
            return -1;
        }

    return found;
}
