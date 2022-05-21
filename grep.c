/* Inspired by K&R C - "The C Progamming Language" */

#define _POSIX_C_SOURCE 200809L  /* for getline() */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "dksh_common.h"

/* prototypes for extern functions */
char * strcasestr(const char *, const char *);

static void clean_up(char *);
static char * my_strstr(const char *, const char *, int);

int grep(int argc, char **args)
{
    /* calloc will initialize the memory to zero */
    char *buf = calloc(BUFSIZE, sizeof(char));
    long lineno = 0;
    int c, found = 0, max_count = -1;
    size_t str_size = 0;
    int except = 0, number = 0, count_only = 0, print_filename = 0, ignore_case = 0;
    int only_matching = 0, quiet = 0, byte_offset = 0;
    int files_without_match = 0, files_with_matches = 0;
    /* used as the 2nd parameter of getline */
    size_t tmp = BUFSIZE;
    FILE *fs;
    char *strstr_result, *filename, *label = NULL;

    /* parse the options */
    while(--argc > 0 && (*++args)[0] == '-') {
        /* prevent the value of args[0] (which is a pointer) from being changed */
        char *saved_args_0 = args[0];

        while((c = *++saved_args_0)) {
            switch(c) {
                /* invert the sense of matching, to select non-matching lines */
                case 'v':  except = 1;  break;
                /* prefix each line of output with the 1-based line number */
                /* within its input file */
                case 'n':  number = 1;  break;
                /* only a count of selected lines is written to standard output */
                case 'c':  count_only = 1;  break;
                /* always print filename headers with output lines */
                case 'H':  print_filename = 1;  break;
                /* never print filename headers (i.e., filenames) with output lines */
                case 'h':  print_filename = 0;  break;
                /* perform case insensitive matching */
                case 'i':  ignore_case = 1;  break;
                /* prints only the matching part of the lines */
                case 'o':  only_matching = 1;  break;
                /* quiet mode: suppress normal output */
                case 'q':  quiet = 1;  break;
                /* the offset in bytes of a matched pattern is displayed */
                /* in front of the respective matched line */
                case 'b':  byte_offset = 1;  break;
                /* only the names of files not containing selected lines are written to */
                /* standard output */
                case 'L':  files_without_match = 1;  files_with_matches = 0;  break;
                /* only the names of files containing selected lines are written to */
                /* standard output */
                case 'l':  files_with_matches = 1;  files_without_match = 0;  break;
                /* stop reading a file after max_count matching lines */
                case 'm':  if (*(saved_args_0 + 1) != '\0') {
                               if (!isdigit(*(saved_args_0 + 1))) {
                                   printf("grep: invalid max count\n");
                                   clean_up(buf);
                                   return 2;
                               }
                               max_count = atoi(saved_args_0 + 1);
                               /* step over the NUM after '-m' */
                               while (*(++saved_args_0 + 1) != '\0') {}
                           } else {
                               if (!isdigit(args[1][0])) {
                                   printf("grep: invalid max count\n");
                                   clean_up(buf);
                                   return 2;
                               }
                               max_count = atoi(*++args);
                               /* step over the NUM after '-m' */
                               --argc;
                           }
                           break;
                /* options starting with '--' */
                case '-':  if (strcmp(saved_args_0 + 1, "label") == 0) {
                               label = *++args;
                               /* step over the remaining of the option '--label' */
                               while (*(++saved_args_0 + 1) != '\0') {}
                               /* step over the LABEL after '--label' */
                               --argc;
                               break;
                           }
                           /* fall through to default */
                default:  printf("grep: illegal option %c\n", c);
                          argc = 0;
                          found = -1;
                          break;
            }
        }
    }

    /* argc and args have been changed through the while body */
    if(argc < 1) {
        printf("Usage: grep -vncHhioqbLl -m <num> --label <LABEL> pattern [file]\n");
        clean_up(buf);
        return 2;
    }
    if (argc == 1) {    /* if no file specified, then use STDIN */
        fs = stdin;
        filename = (label == NULL) ? "(standard input)" : label;
    } else {    /* argc == 2 */
        filename = *(args + 1);
        fs = fopen(filename, "r");
        if (!fs) {
            fprintf(stderr, "grep: %s: No such file or directory\n", *(args + 1));
            clean_up(buf);
            return 2;
        }
    }

    /* <stdio.h>: ssize_t getline(char **lineptr, size_t *n, FILE *stream); */
    while (getline(&buf, &tmp, fs) > 0) {
        if (max_count >= 0 && found >= max_count) {
            break;
        }

        lineno++;

        strstr_result = my_strstr(buf, *args, ignore_case);;
        /* kind of tricky */
        if((strstr_result != NULL) != except) {
            found++;
            if (!quiet && !count_only && !files_without_match && !files_with_matches) {
                if (print_filename) {
                    printf("%s:", filename);
                }

                if (number) {
                    printf("%ld:", lineno);
                }

                if (byte_offset) {
                    if (only_matching) {
                        printf("%ld:", str_size + (strstr_result - buf));
                    } else {
                        printf("%ld:", str_size);
                    }
                }

                if (only_matching) {
                    printf("%s\n", *args);
                } else {
                    printf("%s", buf);
                }
            }

            if (files_with_matches) {
                printf("%s\n", filename);
                break;
            }
        }

        str_size += strlen(buf);

        /* clear buf before reading the next line */
        memset(buf, 0, BUFSIZE);
    }

    if (!quiet && count_only && !files_without_match && !files_with_matches) {
        printf("%d\n", found);
    }

    if (found == 0 && files_without_match) {
        printf("%s\n", filename);
    }

    clean_up(buf);

    if (fs == stdin) {
        clearerr(stdin);    /* prevent the EOF (Ctrl-D) from causing dksh to exit */
    }

    if (fs != stdin) {
        if (fclose(fs) == EOF) {
            perror("fclose");
            return 2;
        }
    }

    return found > 0 ? 0 : 1;
}

static void clean_up(char *buf) {
    if (buf) {
        free(buf);
    }
}

static char * my_strstr(const char *haystack, const char *needle, int ignore_case) {
    if (ignore_case) {
        return strcasestr(haystack, needle);
    }
    return strstr(haystack, needle);
}
