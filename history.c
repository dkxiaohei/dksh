#include <stdio.h>

int history(char **hist)
{
    int i;

    for (i = 0; hist[i] != NULL; i++)
        printf("%-5d %s", i, hist[i]);

    return 0;
}
