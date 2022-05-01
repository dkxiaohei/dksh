#include <stdio.h>
#include <time.h>

/* prototypes for extern functions */
char *ctime_r(const time_t *, char *);

#define BUFSIZE 32

int date(void)
{
    char buf[BUFSIZE];
    time_t t = time(NULL);

    ctime_r(&t, buf);
    printf("%s", buf);

    return 0;
}
