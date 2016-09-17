#include <stdio.h>
#include <time.h>
#include <utmp.h>

int who(int argc, char **args)
{
    struct utmp *ut;
    time_t t;

    // use alternate file if supplied, instead of /var/run/utmp
    if (argc > 1)
        if (utmpname(args[1]) == -1) {
            perror("utmpname");
            return -1;
        }

    setutent();
    printf("USER       PID LINE   HOST            TIME\n");

    while ((ut = getutent()) != NULL)    // sequential scan to EOF
        if (ut->ut_type == 7) {    // only display USER_PROCESS (7)
            printf("%-8s ", ut->ut_user);
            printf("%5ld %-6.6s %-15.15s ", (long) ut->ut_pid,
                    ut->ut_line, ut->ut_host);
            // printf("%s", ctime((time_t *) &(ut->ut_tv.tv_sec)));
            t = ut->ut_tv.tv_sec;
            printf("%s", ctime(&t));
        }

    endutent();

    return 0;
}
