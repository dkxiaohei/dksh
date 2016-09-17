#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>

int my_time(int argc, char **args)
{
    struct tms t_buf;
    pid_t pid;
    int status;
    long clock_tick;
    clock_t before, after;

    clock_tick = sysconf(_SC_CLK_TCK);
    before = times(&t_buf);

    if (argc > 1) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {  // child process
            if (execvp(args[1], &args[1]) == -1) {
                fprintf(stderr, "%s: %s command not found\n",
                        args[0], args[1]);

                _exit(EXIT_FAILURE);  // exit() is unreliable here
            }

            _exit(EXIT_SUCCESS);  // exit() is unreliable here
        }
        else  // pid > 0, parent process
            wait(&status);  // wait for child process to finish
    }

    after = times(&t_buf);
    printf("real %.2f\n", (double)(after - before) / clock_tick);
    printf("user %.2f\n", (double)t_buf.tms_cutime / clock_tick);
    printf("sys  %.2f\n", (double)t_buf.tms_cstime / clock_tick);

    return 0;
}
