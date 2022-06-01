#define _POSIX_C_SOURCE 200809L    /* for sigaction */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <libgen.h>

#include "dksh.h"

/* main */
int main(void)
{
    uid_t uid;
    char hostname[32] = {'\0'}, *prompt;
    int argc, hist_count = 0;

    gethostname(hostname, (size_t)32);
    /* root (#) or normal ($) */
    prompt = (uid = getuid()) == 0 ? "# " : "$ ";
    setuid(uid);
    setgid(getgid());

    /* ignore SIGINT (Ctrl-C), SIGQUIT (Ctrl-\) and SIGTSTP (Ctrl-Z) */
    ignore_signal();

    /* handle SIGCHLD, get noted about the termination of the background command */
    handle_sigchld();

    /* main loop */
    while (1) {
        print_promt(prompt, hostname);
        process_ctrl_d();

        /* get input, parse args and add to history */
        fgets(input, LINEMAX, stdin);
        /* argc is the number of arguments (including the command itself) */
        argc = get_args(input);
        if (argc > 0) {
            record_history(&hist_count);
        }

        /* run the command */
        if (!args[0]) {
            continue;
        }
        /* if the command doesn't start with '-' or '.', then it's a built-in command */
        if (args[0][0] != '-' && args[0][0] != '.') {
            run_built_in_cmd(argc, args);
        } else {    /* otherwise it's a system command or user's command */
            run_system_or_user_cmd();
        }

        free_args();
    }    /* end while (1), the main loop */

    return 0;
}

static void ignore_signal(void)
{
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "Cannot ignore SIGINT!\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "Cannot ignore SIGQUIT!\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
        fprintf(stderr, "Cannot ignore SIGTSTP!\n");
        exit(EXIT_FAILURE);
    }
}

static void handle_sigchld(void)
{
    struct sigaction sa;
    sa.sa_handler = &do_handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

static void do_handle_sigchld(int sig)
{
    int saved_errno = errno;
    pid_t pid;
    char *msg = "\nChild process has terminated\n";
    while ((pid = waitpid((pid_t)(-1), 0, WNOHANG)) > 0) {
        write(1, msg, strlen(msg) + 1);
    }
    errno = saved_errno;
}

static void print_promt(const char *prompt, const char *hostname)
{
    char *cwd, *tmp_cwd;

    cwd = getcwd(NULL, 0);
    tmp_cwd = strdup(cwd);
    printf("[%s@%s:%s] %s", getenv("USER"), hostname, basename(tmp_cwd), prompt);
    free(cwd);
}

static void process_ctrl_d(void)
{
    int ch = getchar();
    if (ch == EOF) {    /* Ctrl-D to exit */
        clean_up();
        printf("\nexit dksh by Ctrl-D\n");
        my_exit();
    } else {    /* if the first char is not EOF, then put it back into the STDIN stream */
        ungetc(ch, stdin);
    }
}

static int is_same_command(int hist_count)
{
    return hist_count > 0 && strcmp(hist[hist_count - 1], input) == 0;
}

static void record_history(int *hist_count)
{
    /* ignore successive same commands */
    if (is_same_command(*hist_count)) {
        return;
    }

    if (!hist[*hist_count]) {
        hist[*hist_count] = malloc((strlen(input) + 1) * sizeof(char));
        if (!hist[*hist_count]) {
            perror("malloc");
            clean_up();
            exit(EXIT_FAILURE);
        }
    } else {
        char *tmp_hist = hist[*hist_count];
        hist[*hist_count] = realloc(tmp_hist, (strlen(input) + 1) * sizeof(char));
        if (!hist[*hist_count]) {
            perror("realloc");
            free(tmp_hist);
            clean_up();
            exit(EXIT_FAILURE);
        }
    }

    strcpy(hist[*hist_count], input);
    *hist_count = (*hist_count + 1) % HISTMAX;
}

static int get_args(const char *input)
{
    int i = 0, j;
    background = FALSE;

    if (!input) {
        return 0;
    }

    while (1) {
        while (*input == ' ') {    /* omit the preceding blank(s) */
            input++;
        }
        if (*input == '\n') {    /* only support one-line command */
            break;
        }

        if ((args[i] = malloc(LINEMAX * sizeof(char))) == NULL) {
            /* if malloc fails */
            free_args();
            return 0;
        }

        j = 0;
        /* suppose arguments are split by blank(s) */
        while (j < LINEMAX - 1 && *input != ' ' && *input != '\n') {
            args[i][j++] = *input++;
        }
        args[i][j] = '\0';    /* terminate each argument with '\0' */

        if (++i > ARGMAX - 1) {    /* too many args */
            free_args();
            return 0;
        }
    }

    if (args[i-1] != NULL && strcmp(args[i-1], "&") == 0) {
        background = TRUE;
        args[i-1] = NULL;
        i--;
    }

    return i;    /* return the number of arguments */
}

static void free_args(void)
{
    int i;
    for (i = 0; i < ARGMAX; i++) {
        if (args[i]) {
            free(args[i]);
            args[i] = NULL;
        }
    }
}

static void free_hist(void)
{
    int i;
    for (i = 0; hist[i] != NULL; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
}

static void clean_up(void) {
    free_args();
    free_hist();
}

static int do_run_built_in_cmd(int argc, char **args)
{
    if (strcmp(args[0], "exit") == 0) {
        clean_up();
        my_exit();
    }
    if (strcmp(args[0], "help") == 0) {
        return help();
    }
    if (strcmp(args[0], "pwd") == 0) {
        return pwd();
    }
    if (strcmp(args[0], "cd") == 0) {
        return cd(args);
    }
    if (strcmp(args[0], "echo") == 0) {
        return echo(argc, args, return_value);
    }
    if (strcmp(args[0], "cat") == 0) {
        return cat(argc, args);
    }
    if (strcmp(args[0], "ls") == 0) {
        return ls(argc, args);
    }
    if (strcmp(args[0], "mkdir") == 0) {
        return my_mkdir(argc, args);
    }
    if (strcmp(args[0], "rmdir") == 0) {
        return my_rmdir(argc, args);
    }
    if (strcmp(args[0], "rm") == 0) {
        return rm(argc, args);
    }
    if (strcmp(args[0], "date") == 0) {
        return date();
    }
    if (strcmp(args[0], "chmod") == 0) {
        return my_chmod(argc, args);
    }
    if (strcmp(args[0], "wc") == 0) {
        return wc(argc, args);
    }
    if (strcmp(args[0], "kill") == 0) {
        return my_kill(argc, args);
    }
    if (strcmp(args[0], "history") == 0) {
        return history(hist);
    }
    if (strcmp(args[0], "who") == 0) {
        return who(argc, args);
    }
    if (strcmp(args[0], "grep") == 0) {
        return grep(argc, args);
    }
    if (strcmp(args[0], "mv") == 0) {
        return mv(argc, args);
    }
    if (strcmp(args[0], "tee") == 0) {
        return tee(argc, args);
    }
    if (strcmp(args[0], "time") == 0) {
        return my_time(argc, args);
    }
    if (strcmp(args[0], "more") == 0) {
        return more(argc, args);
    }
    if (strcmp(args[0], "dcl") == 0) {
        return my_dcl(argc, args);
    }
    if (strcmp(args[0], "undcl") == 0) {
        return undcl(argc, args);
    }
    if (strcmp(args[0], "sleep") == 0) {
        return my_sleep(argc, args);
    }
    /* if the command isn't built-in, then try execvp next time */
    printf("dksh: %s: command not found... (try '-%s')\n", args[0], args[0]);
    return -1;
}

static void run_built_in_cmd(int argc, char **args)
{
    pid_t pid;

    if (!background) {
        return_value = do_run_built_in_cmd(argc, args);
        return;
    }

    if ((pid = fork()) < 0) {
        perror("fork");
        clean_up();
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {    /* child process */
        do_run_built_in_cmd(argc, args);
        _exit(EXIT_SUCCESS);
    } else {    /* pid > 0: parent process */
        /* do nothing */
    }
}

static void run_system_or_user_cmd(void)
{
    pid_t pid;

    if ((pid = fork()) < 0) {    /* if fork fails */
        perror("fork");
        clean_up();
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {    /* child process */
        if (args[0][0] == '-') {    /* call Bash commands */
            ++args[0];    /* omit the preceding '-' */
        }
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "dksh: bash: %s: command not found\n", args[0]);
            _exit(EXIT_FAILURE); /* exit() is unreliable here, so _exit must be used */
        }
    } else {    /* pid > 0: parent process */
        if (!background) {
            wait(0);    /* wait for child process */
        }
    }
}
