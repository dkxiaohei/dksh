#ifndef DKSH_H
#define DKSH_H

#include "dksh_common.h"

#define LINEMAX 80
#define ARGMAX 16
#define HISTMAX 500

/* prototypes for extern functions */
int gethostname(char *, size_t);
char *strdup(const char *);
/* dksh built-in functions */
int pwd(void);
int cd(char **);
int echo(int, char **, int);
int cat(int, char **);
int ls(int, char **);
int my_mkdir(int, char **);
int my_rmdir(int, char **);
int rm(int, char **);
int date(void);
int my_chmod(int, char **);
int wc(int, char **);
int my_kill(int, char **);
int history(char **);
int who(int, char **);
int help(void);    /* list the commands that dksh supports */
int grep(int, char **);
int mv(int, char **);
int tee(int, char **);
int my_time(int, char **);
int more(int, char **);
int my_dcl(int, char **);
int undcl(int, char **);
void my_exit(void);

/* prototypes for file static functions */
static void ignore_signal(void);    /* ignore some signals */
static void handle_sigchld(void);    /* handle SIGCHLD signal */
static void do_handle_sigchld(int);    /* handle SIGCHLD signal */
static void print_promt(const char *, const char *);    /* print the prompt */
static void process_ctrl_d(void);    /* Ctrl-D to exit */
static int is_same_command(int);    /* check if it is the same command */
static void record_history(int *);    /* record the command history */
static int get_args(const char *);    /* parse the input to get args */
static void free_args(void);    /* free the memory of args */
static void free_hist(void);    /* free the memory of hist */
static void clean_up(void);    /* free the memory of args and hist */
static void write_char(int, char);    /* write a char to a file descriptor */
static void run_built_in_cmd(int, char **);    /* run dksh built-in commands */
static void do_run_built_in_cmd(int, char **);    /* run dksh built-in commands */
static void run_system_or_user_cmd(void);    /* run symtem or user's commands */

/* file static variables */
static char input[LINEMAX];
static char *args[ARGMAX];
static char *hist[HISTMAX];
static int return_value;    /* echo $? */
static int background;    /* run cmd in the background */

#endif  /* DKSH_H */
