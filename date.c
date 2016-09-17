#include <stdio.h>
#include <time.h>

int date(void)
{
    char buf[32];
    time_t t = time(NULL);

    ctime_r(&t, buf);
    printf("%s", buf);

    return 0;
}
