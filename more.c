#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#define LINELEN 256
#define PAGELEN 24
#define PAGERPROMPT " --More-- "
#define BACKOVERPROMPT "\b\b\b\b\b\b\b\b\b\b"

static int more_display(FILE *fp)
{ 
    char c;

    printf("\033[7m"PAGERPROMPT"\033[m");
    printf(BACKOVERPROMPT);

    while (1) {
        fflush(0);
        c = fgetc(fp);
        if (c == 'q')
            return 0;
        if (c == ' ')
            return PAGELEN;
        if (c == '\n')
            return 1;
    }

    return 0;
}

int more(int argc, char **args)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", args[0]);
        return -1;
    }

    int i = 0, j = 0;
    char line[LINELEN];
    FILE *fp;

    if ((fp = fopen(args[1], "r")) == NULL) {
        fprintf(stderr, "%s is not a file\n", args[1]);
        return -1;
    }
    else {
        FILE *fp_tty_in, *fp_tty_out;
        fp_tty_in = fopen("/dev/tty", "r");
        fp_tty_out = fopen("/dev/tty", "w");

        struct termios settings, initials;
        tcgetattr(STDIN_FILENO, &initials);
        settings = initials;
        settings.c_lflag &= ~ICANON;
        settings.c_lflag &= ~ECHO;
        settings.c_cc[VMIN] = 1;
        settings.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &settings) != 0)
            fprintf(stderr, "Could not set attributes\n");

        while (fgets(line, LINELEN, fp) != NULL) {
            if (i == PAGELEN) {
                j = more_display(fp_tty_in);
                if (j == 0)
                    break;
                i -= j;
            }
            if (fputs(line, fp_tty_out) == EOF) {
                tcsetattr(STDIN_FILENO, TCSANOW, &settings);
                break;
            }
            i++;
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &initials);
        fclose(fp_tty_in);
        fclose(fp_tty_out);
    }

    fclose(fp);

    return 0;
}
