#include <stdio.h>
#include <time.h>
#ifdef __APPLE__
#include <utmpx.h>
#else
#include <utmp.h>
#endif  /* __APPLE__ */

int who(int argc, char **args)
{
#ifdef __APPLE__
    struct utmpx *ut;
#else
    struct utmp *ut;
#endif  /* __APPLE__ */
    time_t t;

    // use alternate file if supplied, instead of /var/run/utmp
    if (argc > 1)
#ifdef __APPLE__
        if (utmpxname(args[1]) == -1) {
#else
        if (utmpname(args[1]) == -1) {
#endif  /* __APPLE__ */
            perror("utmpname");
            return -1;
        }

#ifdef __APPLE__
    setutxent();
#else
    setutent();
#endif  /* __APPLE__ */
    printf("USER       PID LINE   HOST            TIME\n");

#ifdef __APPLE__
    while ((ut = getutxent()) != NULL)    // sequential scan to EOF
#else
    while ((ut = getutent()) != NULL)    // sequential scan to EOF
#endif  /* __APPLE__ */
        if (ut->ut_type == 7) {    // only display USER_PROCESS (7)
            printf("%-8s ", ut->ut_user);
            printf("%5ld %-6.6s %-15.15s ", (long) ut->ut_pid,
                    ut->ut_line, ut->ut_host);
            // printf("%s", ctime((time_t *) &(ut->ut_tv.tv_sec)));
            t = ut->ut_tv.tv_sec;
            printf("%s", ctime(&t));
        }

#ifdef __APPLE__
    endutxent();
#else
    endutent();
#endif  /* __APPLE__ */

    return 0;
}
