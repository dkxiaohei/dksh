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

int gettoken();
void dcl();
void dirdcl();

int tokentype;  // type of last token
char token[MAXTOKEN];  // last token string
char name[MAXTOKEN];  // identifier name
char datatype[MAXTOKEN];  // data type = char , int , etc.
char out[MAXOUT];  // output string

/* convert declaration to words */
int my_dcl(int argc, char **args)
{
    printf(DCL_PROMT);
    while (gettoken() != EOF) {  // 1st token on line
        printf(DCL_PROMT);
        strcpy(datatype, token);  // is the datatype
        if (tokentype == '\n')
            continue;
        out[0] = '\0';

        dcl();  // parse rest of line

        if (tokentype != '\n') {
            fprintf(stderr, "syntax error\n");
            return 0;
        }

        if (name[0] != '\0')
            printf("%s: %s %s\n", name, out, datatype);

        token[0] = '\0';
        name[0] = '\0';
        datatype[0] = '\0';
        out[0] = '\0';
    }

    return 0;
}

/* return next token */
int gettoken()
{
    int c;
    char *cursor = token;

    while ((c = getchar()) == ' ' || c == '\t')
        ;

    if (c == '(') {
        if ((c = getchar()) == ')') {
            strcpy(token, "()");
            return tokentype = PARAMS;
        } else {
            ungetc(c, stdin);
            return tokentype = '(';
        }
    } else if (c == '[') {
        for (*cursor++ = c; (*cursor++ = getchar()) != ']';)
            ;
        *cursor = '\0';
        return tokentype = BRACKETS;
    } else if (isalpha(c)) {
        for (*cursor++ = c; isalnum(c = getchar());)
            *cursor++ = c;
        *cursor = '\0';
        ungetc(c, stdin);
        return tokentype = NAME;
    } else {
        return tokentype = c;
    }
}

/* dcl: parse a declarator */
void dcl()
{
    int ns;
    for (ns = 0; gettoken() == '*';)  // count of *'s
        ns++;

    dirdcl();

    while (ns-- > 0)
        strcat(out, " pointer to");
}

/* dirdcl: parse a direct declarator */
void dirdcl()
{
    int type;

    if (tokentype == '(') {  // (dcl)
        dcl();
        if (tokentype != ')')
            fprintf(stderr, "error: missing ')'\n");
    } else if (tokentype == NAME)  // variable name
        strcpy(name, token);
    else {
        fprintf(stderr, "error: expected name or (dcl)\n");
    }

    while ((type = gettoken()) == PARAMS || type == BRACKETS)
        if (type == PARAMS)
            strcat(out, " function returning");
        else {
            strcat(out, " array");
            strcat(out, token);
            strcat(out, " of");
        }
}

int undcl(int argc, char **args)
{
    printf(UNDCL_PROMT);

    return 0;
}
