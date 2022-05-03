#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "dksh_common.h"

/* include SUID, SGID, and sticky-bit information in returned string */
#define FP_SPECIAL 1

#define STR_SIZE sizeof("rwxrwxrwx")

static char perm_str[STR_SIZE];

static char *file_perm(mode_t, int);
static char *user_name_from_id(uid_t);
static char *group_name_from_id(gid_t);
static void display_stat(const struct stat *);

int ls(int argc, char **args)
{
    struct stat path_stat;
    /* a dynamic array of struct dirent ** */
    struct dirent ***namelist;

    int print_dirname = FALSE, has_regular = FALSE, return_error = FALSE;
    int ls_all = FALSE, ls_long = FALSE, ls_inode = FALSE, no_scan = FALSE;
    int option, i, j;
    int *sum = NULL, *error = NULL;

    /* parse the options (args will be increased) */
    while (--argc > 0 && (*++args)[0] == '-') {
        /* prevent args[0] (a pointer to char) from being changed */
        char *save_args_0 = args[0];

        while ((option = *++save_args_0)) {
            switch (option) {
                case 'a':  ls_all = TRUE;  break;    /* show hidden files */
                case 'l':  ls_long = TRUE;  break;    /* show details */
                case 'i':  ls_inode = TRUE;  break;    /* show inode */
                /* do not scan the directory */
                case 'd':  no_scan = TRUE;  break;
                default:  fprintf(stderr, "ls: illegal option %c\n"
                                  "Usage: ls [-a -l -i -d] [file(s)]\n",
                                  option);
                          return -1;
            }
        }
    }

    /* if there're multiple directories, print their names respectively */
    if (argc > 1) {
        print_dirname = TRUE;
    }

    /* calloc: initial value set to 0 */
    error = calloc((argc == 0 ? 1 : argc), sizeof(int));
    if (!error) {
        perror("calloc");
        return -1;
    }

    /* the number of directory(ies) is argc */
    /* (which has been decreased when parsing the options) */
    namelist = malloc((argc == 0 ? 1 : argc) * sizeof(struct dirent **));
    if (!namelist) {
        perror("malloc");
        return -1;
    }

    sum = malloc((argc == 0 ? 1 : argc) * sizeof(int));
    if (!sum) {
        perror("malloc");
        return -1;
    }

    for (i = 0; i < (argc == 0 ? 1 : argc); i++) {
        /* read the stat to distinguish regular files from directories */
        if ((stat((argc == 0) ? "." : args[i], &path_stat)) != 0) {
            fprintf(stderr, "ls: stat: %s: No such file or directory\n", args[i]);
            error[i] = TRUE;
            return_error = TRUE;
        }

        /* a directory, scan it */
        if (S_ISDIR(path_stat.st_mode) && !no_scan) {
            /* if no directory is specified (argc == 0), then scan "." */
            sum[i] = scandir((argc == 0 ? "." : args[i]), &namelist[i], NULL, alphasort);

            if (sum[i] < 0) {
                fprintf(stderr, "ls: scandir: %s: No such file or directory\n", args[i]);
                error[i] = TRUE;
                return_error = TRUE;
            }
        } else {    /* not a directory, just print it */
            has_regular = TRUE;

            if (!error[i]) {
                /* set error[i] to TRUE to ignore it in the next step */
                /* when scanning directories */
                error[i] = TRUE;

                if (ls_inode) {
                    printf("%ld ", (long)path_stat.st_ino);
                }

                if (ls_long) {
                    display_stat(&path_stat);
                }

                printf("%s\n", (argc == 0) ? "." : args[i]);
            }
        }
    }

    /* scan directories if -d option is not set */
    if (!no_scan) {
        i = 0;

        do {
            int count = 0, hidden_count = 0;

            /* if the i'th directory cannot be opened, skip it */
            if (!error[i]) {
                char *oldpwd = NULL;
                oldpwd = getcwd(NULL, 0);
                /* in order for stat to work properly */
                chdir(args[i]);
                printf((i == 0 && !has_regular) ? "" : "\n");

                if (print_dirname) {
                    printf("%s:\n", args[i]);
                }

                for (j = 0; j < sum[i]; j++) {
                    /* without "-a" option, do not show hidden files */
                    if (!ls_all && namelist[i][j]->d_name[0] == '.') {
                        hidden_count++;

                        if (namelist[i][j]) {
                            free(namelist[i][j]);
                        }
                        continue;
                    }

                    if ((stat(namelist[i][j]->d_name, &path_stat)) != 0) {
                        perror("stat");
                        return -1;
                    }

                    if (ls_inode) {
                        printf("%ld ", (long)path_stat.st_ino);
                    }

                    if (ls_long) {
                        display_stat(&path_stat);
                    }

                    count++;
                    printf("%-10s%s", namelist[i][j]->d_name,
                            ls_long ? "\n" :
                            (count % 5 == 0 &&
                             count < sum[i] - hidden_count) ?
                            "\n" : "  ");

                    if (namelist[i][j]) {
                        free(namelist[i][j]);
                    }
                }

                if (!ls_long) {
                    printf("\n");
                }

                if (namelist[i]) {
                    free(namelist[i]);
                }

                chdir(oldpwd);
                free(oldpwd);
            }
        } while (++i < argc);
    }

    /* free the allocated memory */
    if (error) {
        free(error);
    }
    if (namelist) {
        free(namelist);
    }

    return return_error ? -1 : 0;
}

static char *file_perm(mode_t perm, int flags)
{
    snprintf(perm_str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
            (perm & S_IRUSR) ? 'r' : '-',
            (perm & S_IWUSR) ? 'w' : '-',
            (perm & S_IXUSR) ?
                (((perm & S_ISUID) && (flags && FP_SPECIAL)) ? 's' : 'x') :
                (((perm & S_ISUID) && (flags && FP_SPECIAL)) ? 'S' : '-'),
            (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
            (perm & S_IXGRP) ?
                (((perm & S_ISGID) && (flags && FP_SPECIAL)) ? 's' : 'x') :
                (((perm & S_ISGID) && (flags && FP_SPECIAL)) ? 'S' : '-'),
            (perm & S_IROTH) ? 'r' : '-',
            (perm & S_IWOTH) ? 'w' : '-',
            (perm & S_IXOTH) ?
                (((perm & S_ISVTX) && (flags && FP_SPECIAL)) ? 't' : 'x') :
                (((perm & S_ISVTX) && (flags && FP_SPECIAL)) ? 'T' : '-'));

    return perm_str;
}

static char *user_name_from_id(uid_t uid)
{
    struct passwd *pwd;
    pwd = getpwuid(uid);

    return !pwd ? NULL : pwd->pw_name;
}

static char *group_name_from_id(gid_t gid)
{
    struct group *grp;
    grp = getgrgid(gid);

    return !grp ? NULL : grp->gr_name;
}

static void display_stat(const struct stat *s)
{
    switch (s->st_mode & S_IFMT) {
        case S_IFREG:  putchar('-');  break;
        case S_IFDIR:  putchar('d');  break;
        case S_IFCHR:  putchar('c');  break;
        case S_IFBLK:  putchar('b');  break;
        case S_IFLNK:  putchar('l');  break;
        case S_IFIFO:  putchar('p');  break;
        case S_IFSOCK:  putchar('s');  break;
        default:  putchar('?');  break;
    }

    printf("%s ", file_perm(s->st_mode, 1));
    printf("%3ld ", (long)s->st_nlink);
    printf("%s %s ", user_name_from_id(s->st_uid), group_name_from_id(s->st_gid));
    /* printf("%9lld ", (long long)s->st_size); */
    printf("%9ld ", (long)s->st_size);
    printf("%.20s ", ctime(&s->st_mtime) + 4);
}
