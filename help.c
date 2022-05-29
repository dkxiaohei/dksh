#include <stdio.h>

int help(void)
{
    printf("\n--- dksh commands ---\n"
            "help\n"
            "exit (Ctrl-D)\n"
            "pwd\n"
            "cd (-)\n"
            "echo ($?)\n"
            "cat (-n -b -s | -nbs)\n"
            "more\n"
            "ls (-a -d -i -l | -adil)\n"
            "mkdir\n"
            "rmdir\n"
            "rm\n"
            "date\n"
            "time\n"
            "chmod\n"
            "wc (-)\n"
            "kill (-signum)\n"
            "history\n"
            "who\n"
            "grep (-v -n -c -H -h -i -o -q -b -L -l -m <num> --label <LABEL> | -vncHhioqbLl -m <num> --label <LABEL>)\n"
            "mv\n"
            "tee (-a)\n"
            "dcl\n"
            "undcl\n"
            "sleep <NUMBER>\n"
            "./<user's program>\n\n");

    return 0;
}
