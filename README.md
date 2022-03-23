## dksh: a rudimentary shell

This repository is inspired by [Yash: Yet another shell](https://www.samiam.org/software/yash.html)

### dksh commands:

    1. ls (-a -d -i -l | -adil)
    2. echo ($?)
    3. cat
    4. mkdir
    5. rmdir
    6. rm
    7. cd (-)
    8. chmod
    9. pwd
    10. wc (-)
    11. date
    12. exit (Ctrl-D)
    13. help
    14. kill (-signum)
    15. who
    16. history
    17. more
    18. grep (-v -n -c -H -h -m | -vncm)
    19. mv
    20. tee (-a)
    21. time
    22. ./<user's program>

    (PS. Putting a '-' before a command will call the Bash built-in commands.)

### Compile:

This repository has been tested on Ubuntu and CentOS. It is currently not compatible with macOS.

    1. make (generate the target 'dksh')
    2. make debug (generate the target 'dksh', with debug info)
    3. make clean (delete the target 'dksh')

### Manual:

    1. ls:
        (1) ls <directory(ies) | file(s)>
        (2) ls -a <directory(ies) | file(s)>: including the hidden files
        (3) ls -d <directory(ies) | file(s)>: do not scan the directory, merely print
        (4) ls -i <directory(ies) | file(s)>: show inode
        (5) ls -l <directory(ies) | file(s)>: show details

    2. echo:
        (1) echo <string(s)>
        (2) echo $?: show the return value of the last command

    3. cat:
        (1) cat <file(s)>

    4. mkdir:
        (1) mkdir <directory>

    5. rmdir:
        (1) rmdir <directory>

    6. rm:
        (1) rm <file>

    7. cd:
        (1) cd: chdir to $HOME directory
        (2) cd <directory>
        (3) cd -: chdir to the last directory

    8. chmod:
        (1) chmod <mode (such as 664)> <file>

    9. pwd:
        (1) pwd

    10. wc:
        (1) wc: read from STDIN
        (2) wc -: read from STDIN
        (2) wc <file>

    11. date:
        (1) date

    12. exit:
        (1) exit
        (2) Ctrl-D

    13. help:
        (1) help: list the commands that dksh supports

    14. kill:
        (1) kill <pid>: default signal is 15 (SIGTERM)
        (2) kill -<signum (such as 9)> <pid>: specify a signal

    15. who:
        (1) who: show who is logged on

    16. history:
        (1) history: show the commands that have been inputted

    17. more:
        (1) more <file>: file perusal filter for crt viewing

    18. grep:
        (1) grep <pattern> <file (if not specified, STDIN is used)>
        (2) grep -v <pattern> <file>: find the unmatched lines
        (3) grep -n <pattern> <file>: show the line number
        (4) grep -c <pattern> <file>: only a count of selected lines is written to standard output
        (5) grep -H <pattern> <file>: always print filename headers with output lines
        (6) grep -h <pattern> <file>: never print filename headers (i.e., filenames) with output lines
        (7) grep -m <NUM> <pattern> <file>: stop reading a file after NUM matching lines
        (8) grep -vncm <NUM> <pattern> <file>

    19. mv:
        (1) mv <oldpath> <newpath>

    20. tee:
        (1) tee <file (if not specified, STDIN is used)>
        (2) tee -a <file>: append mode rather than truncate mode

    21. time:
        (1) time <command>: run programs and summarize system resource usage

    22. ./<user's program>:
        (1) ./<user's program>

### Todo:
    1. support '>' (IO redirection)
    2. support '>>' (IO redirection, append-only)
    3. support '|' (pipeline)
