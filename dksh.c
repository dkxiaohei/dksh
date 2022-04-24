#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <libgen.h>

#define FALSE 0
#define TRUE 1
#define LINEMAX 80
#define ARGMAX 16
#define BUFSIZE 512
#define HISTMAX 500

/* file static variables */
static char input[LINEMAX] = {'\0'};
static char *args[ARGMAX] = {NULL};
static int return_value = 0;    /* echo $? */
static char *hist[HISTMAX] = {NULL};
/* static int background = FALSE; */

/* prototypes for file static functions */
static int get_args(char *);    /* parse the input to get args */
static void free_args(char **);    /* free the memory of args */
static void free_hist(char **);    /* free the memory of hist */
static void clean_up();    /* free the memory of args and hist */
static void built_in(int, char **);    /* dksh built-in commands */
static void ignore_signal();    /* ignore some signals */

/* prototypes for extern functions */
int pwd();
int cd(char **);
int echo(int, char **, int);
int cat(int, char **);
int ls(int, char **);
int my_mkdir(int, char **);
int my_rmdir(int, char **);
int rm(int, char **);
int date();
int my_chmod(int, char **);
int wc(int, char **);
int my_kill(int, char **);
int history(char **);
int who(int, char **);
int help();    /* list the commands that dksh supports */
int grep(int, char **);
int mv(int, char **);
int tee(int, char **);
int my_time(int, char **);
int more(int, char **);
int my_dcl(int, char **);
int undcl(int, char **);
void my_exit();

/* main */
int main(void)
{
    char *prompt = NULL;
    char hostname[32] = {'\0'};
    gethostname(hostname, (size_t)32);

    uid_t uid;
    /* root (#) or normal ($) */
    prompt = (uid = getuid()) == 0 ? "# " : "$ ";
    setuid(uid);
    setgid(getgid());

    /* pipe (IPC): child process passes the return_value to parent process */
    int pipefd[2];
    char pipech;    /* represent the return_value, 0 or 1 */
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int hist_count = 0;
    int same_command = FALSE;

    /* main loop */
    while (1) {
        /* ignore SIGINT (Ctrl-C), SIGQUIT (Ctrl-\) and SIGTSTP (Ctrl-Z) */
        ignore_signal();

        /* TODO */
        /* handle SIGCHLD here */
        /* get noted about the termination of the background command */

        /* print the prompt */
        char *pwd = getcwd(NULL, 0);
        char *tmp_pwd = strdup(pwd);
        printf("[%s@%s:%s] %s", getenv("USER"), hostname, basename(tmp_pwd), prompt);
        free(pwd);

        /* Ctrl-D to exit */
        int ch = getchar();
        if (ch == EOF) {
            clean_up();
            printf("\nexit dksh by Ctrl-D\n");
            my_exit();
        } else    /* if the first char is not EOF, then put it back into the STDIN stream */
            ungetc(ch, stdin);

        /* get input, add to history and parse args */
        fgets(input, LINEMAX, stdin);

        /* ignore successive same commands */
        if (hist_count > 0 && strcmp(hist[hist_count - 1], input) == 0)
            same_command = TRUE;
        else
            same_command = FALSE;
        if (!same_command) {
            if (!hist[hist_count]) {
                hist[hist_count] = malloc((strlen(input) + 1) * sizeof(char));
                if (!hist[hist_count]) {
                    perror("malloc");
                    clean_up();
                    exit(EXIT_FAILURE);
                }
            } else {
                char *tmp_hist = hist[hist_count];
                hist[hist_count] = realloc(tmp_hist, (strlen(input) + 1) * sizeof(char));
                if (!hist[hist_count]) {
                    perror("realloc");
                    free(tmp_hist);
                    clean_up();
                    exit(EXIT_FAILURE);
                }
            }
            strcpy(hist[hist_count], input);
            hist_count = (hist_count + 1) % HISTMAX;
        }

        /* argc is the number of arguments (including the command itself) */
        int argc = get_args(input);

        /* run the command */
        pid_t pid;
        if (args[0]) {
            /* if the command doesn't start with '-' or '.' */
            /* then it's a built-in command, otherwise fork and execvp */
            if (args[0][0] != '-' && args[0][0] != '.')
                built_in(argc, args);
            else if ((pid = fork()) < 0) {    /* if fork fails */
                perror("fork");
                clean_up();
                exit(EXIT_FAILURE);
            } else if (pid == 0) {    /* child process */
                close(pipefd[0]);    /* close unused read end of pipe */
                if (args[0][0] == '-') {    /* call Bash commands */
                    /* &args[0][1]: excluding the preceding '-' */
                    if (execvp(&args[0][1], args) == -1) {
                        write(pipefd[1], "1", 1);
                        close(pipefd[1]);    /* reader will see EOF */
                        fprintf(stderr, "bash: %s: command not found\n", &args[0][1]);
                        /* exit() is unreliable here, so _exit must be used */
                        _exit(EXIT_FAILURE);
                    } else {
                        write(pipefd[1], "0", 1);
                        close(pipefd[1]);
                    }
                } else if (args[0][0] == '.') {    /* user's program ("./<program name>") */
                    if (execvp(args[0], args) == -1) {
                        write(pipefd[1], "1", 1);
                        close(pipefd[1]);    /* reader will see EOF */
                        fprintf(stderr, "bash: %s: command not found\n", args[0]);
                        /* exit() is unreliable here, so _exit must be used */
                        _exit(EXIT_FAILURE);
                    } else {
                        write(pipefd[1], "0", 1);
                        close(pipefd[1]);
                    }
                }
                _exit(EXIT_SUCCESS);
            } else {    /* pid > 0: parent process */
                close(pipefd[1]);    /* close unused write end of pipe */
                read(pipefd[0], &pipech, 1);
                close(pipefd[0]);
                return_value = (pipech == '1') ? -1 : 0;
                wait(0);    /* wait for child */
            }
        }
        free_args(args);
    }    /* end while (1), the main loop */

    return 0;
}

static int get_args(char *input)
{
    if (!input)
        return 0;

    int i = 0, j;

    while (1) {
        while (*input == ' ')    /* omit the preceding blank(s) */
            input++;

        if (*input == '\n')    /* only support one-line command */
            break;

        if ((args[i] = malloc(LINEMAX * sizeof(char))) == NULL) {
            /* if malloc fails */
            free_args(args);
            return 0;
        }

        j = 0;
        /* suppose arguments are split by blank(s) */
        while (j < LINEMAX - 1 && *input != ' ' && *input != '\n')
            args[i][j++] = *input++;
        args[i][j] = '\0';    /* terminate each argument with '\0' */

        if (++i > ARGMAX - 1) {    /* too many args */
            free_args(args);
            return 0;
        }
    }

    return i;    /* return the number of arguments */
}

static void free_args(char **args)
{
    if (!args)
        return;

    int i;
    for (i = 0; i < ARGMAX; i++)
        if (args[i]) {
            free(args[i]);
            args[i] = NULL;
        }
}

static void free_hist(char **hist)
{
    if (!hist)
        return;

    int i;
    for (i = 0; hist[i] != NULL; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
}

static void clean_up() {
    free_args(args);
    free_hist(hist);
}

static void built_in(int argc, char **args)
{
    if (strcmp(args[0], "help") == 0)
        return_value = help();
    else if (strcmp(args[0], "exit") == 0) {
        clean_up();
        my_exit();
    } else if (strcmp(args[0], "pwd") == 0)
        return_value = pwd();
    else if (strcmp(args[0], "cd") == 0)
        return_value = cd(args);
    else if (strcmp(args[0], "echo") == 0)
        return_value = echo(argc, args, return_value);
    else if (strcmp(args[0], "cat") == 0)
        return_value = cat(argc, args);
    else if (strcmp(args[0], "ls") == 0)
        return_value = ls(argc, args);
    else if (strcmp(args[0], "mkdir") == 0)
        return_value = my_mkdir(argc, args);
    else if (strcmp(args[0], "rmdir") == 0)
        return_value = my_rmdir(argc, args);
    else if (strcmp(args[0], "rm") == 0)
        return_value = rm(argc, args);
    else if (strcmp(args[0], "date") == 0)
        return_value = date();
    else if (strcmp(args[0], "chmod") == 0)
        return_value = my_chmod(argc, args);
    else if (strcmp(args[0], "wc") == 0)
        return_value = wc(argc, args);
    else if (strcmp(args[0], "kill") == 0)
        return_value = my_kill(argc, args);
    else if (strcmp(args[0], "history") == 0)
        return_value = history(hist);
    else if (strcmp(args[0], "who") == 0)
        return_value = who(argc, args);
    else if (strcmp(args[0], "grep") == 0)
        return_value = grep(argc, args);
    else if (strcmp(args[0], "mv") == 0)
        return_value = mv(argc, args);
    else if (strcmp(args[0], "tee") == 0)
        return_value = tee(argc, args);
    else if (strcmp(args[0], "time") == 0)
        return_value = my_time(argc, args);
    else if (strcmp(args[0], "more") == 0)
        return_value = more(argc, args);
    else if (strcmp(args[0], "dcl") == 0)
        return_value = my_dcl(argc, args);
    else if (strcmp(args[0], "undcl") == 0)
        return_value = undcl(argc, args);
    else {
        /* if the command isn't built-in, then try execvp next time */
        printf("dksh: %s: command not found...(try '-%s')\n", args[0], args[0]);
        return_value = -1;
    }
}

static void ignore_signal()
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
