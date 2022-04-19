#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define BUFSIZE 512

static char buf[BUFSIZE];
static int print_lineno = FALSE;
static int print_non_blank_lineno = FALSE;
static int lineno;

static int do_cat(FILE *);

int cat(int argc, char **args)
{
    FILE *fp;
    int opt;
    int return_error = FALSE;

    while ((opt = getopt(argc, args, "nb")) != -1)
        switch (opt) {
            case 'n':
                print_lineno = TRUE;
                break;
            case 'b':
                print_non_blank_lineno = TRUE;
                break;
            default:
                fprintf(stderr, "Unsupported option: %d\n", opt);
                return-1;
        }
    argc -= optind;
    args += optind;

    if (argc == 0)
        do_cat(stdin);
    else
        while (argc-- > 0) {
            if ((fp = fopen(*args++, "r")) == NULL) {
                fprintf(stderr, "cat: %s: No such file or directory\n", *args);
                return_error = TRUE;
                continue;    // try the next file (if any)
            }
            do_cat(fp);
        }

    return return_error ? -1 : 0;
}

static int do_cat(FILE *fp)
{
    lineno = 0;

    while ((fgets(buf, sizeof(buf), fp)) != NULL) {
        if (print_non_blank_lineno && buf[0] != '\n')
            printf("%6d\t", ++lineno);
        else if (!print_non_blank_lineno && print_lineno)
            printf("%6d\t", ++lineno);

        if ((fputs(buf, stdout)) == EOF) {
            perror("fputs");
            return -1;
        }
    }

    if (fclose(fp) == EOF) {
        perror("fclose");
        return -1;
    }

    return 0;
}
