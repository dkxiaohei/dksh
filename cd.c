#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int cd(char **args)
{
    // static local variable, to preserve the old pwd
    static char *oldpwd = NULL;
    char *tmp = NULL;
    tmp = getcwd(NULL, 0);

    // if called with no arguments, then chdir to $HOME
    if (!args[1]) {
        if (chdir(getenv("HOME")) == -1) {
            perror("chdir($HOME)");
            return -1;
        }
    }
    // if called with '-', then chdir to $OLDPWD
    else if (args[1][0] == '-' && args[1][1] == '\0') {
        if (chdir(oldpwd) == -1) {
            perror("chdir($OLDPWD)");
            return -1;
        }
    }
    else {
        if (chdir(args[1]) == -1) {
            fprintf(stderr,
                    "cd: %s: No such file or directory\n",
                    args[1]);
            return -1;
        }
    }

    free(oldpwd);
    oldpwd = tmp;

    return 0;
}
