#include <stdio.h>

int help(void)
{
    printf("--- BUILT-IN COMMANDS ---\n"
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
            "wc\n"
            "kill (-sig)\n"
            "history\n"
            "who\n"
            "./<user's program>\n");

    return 0;
}
