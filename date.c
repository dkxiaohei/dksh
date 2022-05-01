#include <stdio.h>
#include <time.h>

#define BUFSIZE 32

int date(void)
{
    char buf[BUFSIZE];
    time_t t = time(NULL);

    ctime_r(&t, buf);
    printf("%s", buf);

    return 0;
}
