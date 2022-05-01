#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int pwd(void)
{
    char *cwd = getcwd(NULL, 0);

    if (!cwd) {
        perror("getcwd");
        return -1;
    }

    printf("%s\n", cwd);
    free(cwd);

    return 0;
}
