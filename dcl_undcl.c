/*
 * Inspired by K&R C - "The C Programming Language"
 *
 * dcl:         optional *'s direct-dcl
 * direct-dcl:  name
 *              (dcl) 
 *              direct-dcl()
 *              direct-dcl[optional size]
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXTOKEN 100
#define MAXOUT 1000
#define DCL_PROMT "dcl> "
#define UNDCL_PROMT "undcl> "

enum { NAME, PARAMS, BRACKETS };

static int gettoken(void);
static int dcl(void);
static int dirdcl(void);
static void clear(void);

static int tokentype;  /* type of last token */
static char token[MAXTOKEN];  /* last token string */
static char name[MAXTOKEN];  /* identifier name */
static char datatype[MAXTOKEN];  /* data type = char , int , etc. */
static char out[MAXOUT];  /* output string */

/* convert declaration to words */
int my_dcl(int argc, char **args)
{
    int result;
    printf(DCL_PROMT);
    while (gettoken() != EOF) {  /* 1st token on line */
        strcpy(datatype, token);  /* is the datatype */
        if (tokentype == '\n') {
            printf(DCL_PROMT);
            continue;
        }
        out[0] = '\0';

        result = dcl();  /* parse rest of line */
        if (result != 0) {
            printf(DCL_PROMT);
            continue;
        }

        if (tokentype != '\n') {
            fprintf(stderr, "syntax error\n");
            continue;
        }

        if (name[0] != '\0') {
            printf("%s: %s %s\n", name, out, datatype);
        }

        clear();
        printf(DCL_PROMT);
    }

    return 0;
}

/* convert word description to declaration */
int undcl(int argc, char **args)
{
    int type;
    char temp[MAXTOKEN + MAXOUT];

    printf(UNDCL_PROMT);
    while (gettoken() != EOF) {
        if (tokentype == '\n') {
            printf(UNDCL_PROMT);
            continue;
        }

        out[0] = '\0';
        strcpy(out, token);

        while ((type = gettoken()) != '\n') {
            if (type == PARAMS || type == BRACKETS) {
                strcat(out, token);
            } else if (type == '*') {
                sprintf(temp, "(*%s)", out);
                strcpy(out, temp);
            } else if (type == NAME) {
                sprintf(temp, "%s %s", token, out);
                strcpy(out, temp);
            } else {
                fprintf(stderr, "invalid input at %s\n", token);
            }
        }

        if (out[0] != '\0') {
            printf("%s\n", out);
        }

        clear();
        printf(UNDCL_PROMT);
    }

    return 0;
}

static void clear(void)
{
    token[0] = '\0';
    name[0] = '\0';
    datatype[0] = '\0';
    out[0] = '\0';
}

/* return next token */
static int gettoken(void)
{
    int c;
    char *cursor = token;

    while ((c = getchar()) == ' ' || c == '\t') {}

    if (c == '(') {
        if ((c = getchar()) == ')') {
            strcpy(token, "()");
            return tokentype = PARAMS;
        }
        ungetc(c, stdin);
        return tokentype = '(';
    }
    if (c == '[') {
        for (*cursor++ = c; (*cursor++ = getchar()) != ']';) {}
        *cursor = '\0';
        return tokentype = BRACKETS;
    }
    if (isalpha(c)) {
        for (*cursor++ = c; isalnum(c = getchar());) {
            *cursor++ = c;
        }
        *cursor = '\0';
        ungetc(c, stdin);
        return tokentype = NAME;
    }
    return tokentype = c;
}

/* dcl: parse a declarator */
static int dcl(void)
{
    int result, ns;
    for (ns = 0; gettoken() == '*';) {  /* count of *'s */
        ns++;
    }

    result = dirdcl();
    if (result != 0) {
        return result;
    }

    while (ns-- > 0) {
        strcat(out, " pointer to");
    }

    return 0;
}

/* dirdcl: parse a direct declarator */
static int dirdcl(void)
{
    int type, result;

    if (tokentype == '(') {  /* (dcl) */
        result = dcl();
        if (result != 0) {
            return result;
        }
        if (tokentype != ')') {
            fprintf(stderr, "error: missing ')'\n");
            return 1;
        }
    } else if (tokentype == NAME) {  /* variable name */
        strcpy(name, token);
    } else {
        fprintf(stderr, "error: expected name or (dcl)\n");
        return 1;
    }

    while ((type = gettoken()) == PARAMS || type == BRACKETS) {
        if (type == PARAMS) {
            strcat(out, " function returning");
        } else {
            strcat(out, " array");
            strcat(out, token);
            strcat(out, " of");
        }
    }

    return 0;
}
