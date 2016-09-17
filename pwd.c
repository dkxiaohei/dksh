#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int pwd(void)
{
    char *pwd = getcwd(NULL, 0);

    if (!pwd) {
        perror("getcwd");
        return -1;
    }

    printf("%s\n", pwd);
    free(pwd);

    return 0;
}
