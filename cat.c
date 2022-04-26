#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dksh.h"

static char buf[BUFSIZE];
static int print_lineno;
static int print_non_blank_lineno;
static int squeeze_empty_lines;
static int is_last_line_empty;
static int lineno;

static void init();
static int do_cat(FILE *);

int cat(int argc, char **args)
{
    init();

    FILE *fp;
    int opt;
    int return_error = FALSE;

    opterr = 0;
    optind = 1;

    while ((opt = getopt(argc, args, "nbs")) != -1)
        switch (opt) {
            case 'n':
                print_lineno = TRUE;
                break;
            case 'b':
                print_non_blank_lineno = TRUE;
                break;
            case 's':
                squeeze_empty_lines = TRUE;
                break;
            default:
                fprintf(stderr, "Unsupported option: %d\n", opt);
                return-1;
        }
    argc -= optind;
    args += optind;

    if (argc == 0) {
        return_error = do_cat(stdin);
        return return_error ? -1 : 0;
    }

    while (argc-- > 0) {
        if ((fp = fopen(*args++, "r")) == NULL) {
            fprintf(stderr, "cat: %s: No such file or directory\n", *args);
            return_error = TRUE;
            continue;    /* try the next file (if any) */
        }
        return_error = do_cat(fp);
    }

    return return_error ? -1 : 0;
}

static void init()
{
    print_lineno = FALSE;
    print_non_blank_lineno = FALSE;
    squeeze_empty_lines = FALSE;
    is_last_line_empty = FALSE;
}

static int do_cat(FILE *fp)
{
    lineno = 0;

    while ((fgets(buf, sizeof(buf), fp)) != NULL) {
        if (buf[0] == '\n' && is_last_line_empty && squeeze_empty_lines)
            continue;

        if (buf[0] == '\n')
            is_last_line_empty = TRUE;
        else
            is_last_line_empty = FALSE;

        if (print_non_blank_lineno && buf[0] != '\n')
            printf("%6d\t", ++lineno);
        else if (!print_non_blank_lineno && print_lineno)
            printf("%6d\t", ++lineno);

        if ((fputs(buf, stdout)) == EOF) {
            perror("fputs");
            return -1;
        }

        memset(buf, 0, sizeof(buf));
    }

    if (fclose(fp) == EOF) {
        perror("fclose");
        return -1;
    }

    return 0;
}
