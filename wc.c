#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int wc(int argc, char **args)
{
    char buf[512];
    int line_num = 0, word_num = 0, char_num = 0;
    int i, n, in_word = 0;
    int fd;

    if (argc < 2 || strcmp(args[1], "-") == 0)
        fd = 0;  // STDIN
    else {
        fd = open(args[1], O_RDONLY);
        if (fd == -1) {
            perror("open");
            return -1;
        }
    }

    while ((n = read(fd, buf, sizeof(buf))) > 0)
        for (i = 0; i < n; i++) {
            char_num++;
            if (buf[i] == '\n')
                line_num++;

            if (strchr(" \r\t\n\v", buf[i]))
                in_word = 0;
            else if (!in_word) {
                word_num++;
                in_word = 1;
            }
        }

    printf("line: %d  word: %d  char: %d  %s\n",
            line_num, word_num, char_num,
            fd == 0 ? "(STDIN)" : args[1]);

    if (fd != 0)  // not STDIN
        if (close(fd) == -1) {
            perror("close");
            return -1;
        }

    return 0;
}
