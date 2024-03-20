#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define SAFEALLOC(var, Type)                          \
    if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
        err("not enough memory");

enum
{
    ID,
    END,
    CT_INT,
    CT_REAL,
    CT_CHAR,
};

typedef struct _Token
{
    int code; // code (name)
    union
    {
        char *text; // used for ID
        long int i; // used for CT_INT
        double r;   // used for CT_REAL
    };
    int line;            // the input file line
    struct _Token *next; // link to the next token
} Token;

Token *tokens = NULL;
Token *lastToken = NULL;

int line = 1;
const char *pCrtCh;

void err(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

Token *addTk(int code, int line)
{
    Token *tk;
    SAFEALLOC(tk, Token)
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastToken)
    {
        lastToken->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastToken = tk;
    return tk;
}

char *createString(const char *start, const char *end)
{
    int length = end - start;
    char *str = (char *)malloc(length + 1);
    if (str == NULL)
    {
        err("not enough memory");
    }
    strncpy(str, start, length);
    str[length] = '\0';
    return str;
}

int getNextToken()
{
    int state = 0;
    char ch;
    const char *pStartCh;
    Token *tk;
    while (1)
    {
        ch = *pCrtCh;
        printf("ch %c\n", ch);
        printf("state %d\n", state);
        switch (state)
        {
        case 0:
            printf("CASE 1\n");
            if (isalpha(ch) || ch == '_')
            {
                pStartCh = pCrtCh; // memorizes the beginning of the ID
                pCrtCh++;          // consume the character
                state = 1;         // set the new state
            }
            else if (isdigit(ch))
            {
                pStartCh = pCrtCh;
                pCrtCh++;
                if (ch >= '1' && ch <= '9')
                {
                    printf("via 3\n");
                    state = 3;
                }
                else
                {
                    printf("via 5\n");
                    state = 5;
                }
            }
            else if (ch == ' ' || ch == '\r' || ch == '\t')
            {
                pCrtCh++;
            }
            else if (ch == '\n')
            {
                line++;
                pCrtCh++;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character");
            }
            break;
        case 1:
            printf("CASE 1\n");
            if (isalnum(ch) || ch == '_')
            {
                pCrtCh++;
                state = 2;
            }
            else
            {
                tkerr(addTk(END, line), "invalid real number format");
            }
            break;
        case 2:
            printf("CASE 2\n");
            if (isalnum(ch) || ch == '_')
            {
                pCrtCh++;
            }
            else
            {
                tk = addTk(ID, line);
                tk->text = createString(pStartCh, pCrtCh);
                return tk->code;
            }
            break;
        case 3:
            printf("CASE 3\n");
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else if (ch == '.')
            {
                printf(". read\n");
                state = 8;
            }
            else if (ch == 'e' || ch == 'E')
            {
                printf("e/E read\n");
                state = 13;
            }
            else
            {
                state = 4;
            }
            break;
        case 4:
            printf("CASE 4\n");
            if (isdigit(ch))
                pCrtCh++;
            else
            {
                printf("final int\n");
                tk = addTk(CT_INT, line);
                tk->i = strtol(pStartCh, NULL, 10);
                return tk->code;
            }
            break;
        case 5:
            if (ch == 'x')
            {
                printf("x read\n");
                state = 6;
            }
            else if (ch == '.')
            {
                printf(". read\n");
                pCrtCh++;
                state = 8;
            }
            else if (ch >= '0' && ch <= '7')
            {
                printf("between 0 and 7\n");
                pCrtCh++;
            }
            else
            {
                printf("8 or 9 was read\n");
                state = 3;
            }
            break;
        case 6:
            if (isalnum(ch) || isdigit(ch))
            {
                printf("is alpha\n");
                pCrtCh++;
            }
            else
            {
                printf("final int\n");
                tk = addTk(CT_INT, line);
                tk->i = strtol(pStartCh, NULL, 10);
                return tk->code;
            }
            break;
        case 8:
            printf("CASE 8\n");
            if (isdigit(ch))
            {
                printf("is digit\n");
                state = 9;
            }
            break;
        case 9:
            printf("CASE 9\n");
            if (isdigit(ch))
            {
                printf("is digit\n");
                pCrtCh++;
            }
            else
            {
                tk = addTk(CT_REAL, line);
                tk->r = atof(pStartCh);
                return tk->code;
            }
            break;
        }
    }
}

void showTokens()
{
    Token *current_token = tokens;
    while (current_token != NULL)
    {
        printf("Token code: %d, Line: %d\n", current_token->code, current_token->line);
        if (current_token->code == ID)
        {
            printf("ID: %s\n", current_token->text);
        }
        else if (current_token->code == CT_INT)
        {
            printf("CT_INT: %ld\n", current_token->i);
        }
        else if (current_token->code == CT_REAL)
        {
            printf("CT_REAL: %f\n", current_token->r);
        }
        current_token = current_token->next;
    }
}

int main()
{

    const char input[] = "0123 hello029_ \n 02323.93208";
    pCrtCh = input;

    while (*pCrtCh != '\0')
    {
        getNextToken();
    }

    showTokens();

    // done() here i have to free the memory

    return 0;
}
