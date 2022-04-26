#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "dksh.h"

static int total_line_num;
static int total_word_num;
static int total_char_num;

static void init();
static int do_wc(int, char *);

int wc(int argc, char **args)
{
    init();

    if (argc < 2 || strcmp(args[1], "-") == 0)
        return do_wc(0, NULL);  /* STDIN */

    while (--argc > 0) {
        char *filename = *++args;
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("open");
            return -1;
        }

        do_wc(fd, filename);

        if (fd != 0)  /* not STDIN */
            if (close(fd) == -1) {
                perror("close");
                return -1;
            }
    }

    printf("line: %d  word: %d  char: %d  %s\n",
            total_line_num, total_word_num, total_char_num, "total");

    return 0;
}

static void init()
{
    total_line_num = 0;
    total_word_num = 0;
    total_char_num = 0;
}

static int do_wc(int fd, char *filename)
{
    char buf[BUFSIZE];
    int line_num = 0, word_num = 0, char_num = 0;
    int i, n, in_word = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0)
        for (i = 0; i < n; i++) {
            char_num++;
            total_char_num++;

            if (buf[i] == '\n') {
                line_num++;
                total_line_num++;
            }

            if (strchr(" \r\t\n\v", buf[i]))
                in_word = 0;
            else if (!in_word) {
                word_num++;
                total_word_num++;
                in_word = 1;
            }
        }

    printf("line: %d  word: %d  char: %d  %s\n",
            line_num, word_num, char_num, fd == 0 ? "(STDIN)" : filename);

    return 0;
}
