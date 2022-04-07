#include <stdio.h>

int help(void)
{
    printf("\n--- dksh commands (22 + 1) ---\n"
            "help\n"
            "exit (Ctrl-D)\n"
            "pwd\n"
            "cd (-)\n"
            "echo ($?)\n"
            "cat\n"
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
            "grep (-v -n -c -H -h -i -o -q -b -L -l -m <num> --label <LABEL> | -vncHhioqbLlm <num> --label <LABEL>)\n"
            "mv\n"
            "tee (-a)\n"
            "cdecl\n"
            "./<user's program>\n\n");

    return 0;
}
