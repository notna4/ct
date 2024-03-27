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
    CT_STRING,
    COMMA,
    SEMICOLON,
    LPAR,
    RPAR,
    LBRACKET, // 10
    RBRACKET,
    LACC,
    RACC,
    ADD,
    SUB,
    MUL,
    DIV,
    DOT,
    AND,
    OR, // 20
    NOT,
    ASSIGN,
    EQUAL,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,
    LINECOMMENT,
    COMMENT, // 30
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
            printf("CASE 0\n");
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
            else if (ch == '\'')
            {
                printf("start of ct_char\n");
                // state = 18;
                char *aux = pCrtCh + 1;
                pCrtCh++;
                if (*aux == '\\')
                {
                    printf("ESC\n");
                    state = 20;
                }
                else if (*aux != '\'' && *aux != '\\')
                {
                    printf("NOT ESC\n");
                    pCrtCh++;
                    state = 18;
                }
                else
                {
                    tkerr(addTk(END, line), "invalid char");
                }
            }
            else if (ch == '\"')
            {
                printf("start of ct_string\n");
                // state = 18;
                char *aux = pCrtCh + 1;
                pCrtCh++;
                if (*aux == '\\')
                {
                    printf("ESC\n");
                    state = 20;
                }
                else if (*aux != '\"' && *aux != '\\')
                {
                    printf("NOT ESC\n");
                    state = 22;
                }
                else
                {
                    tkerr(addTk(END, line), "invalid string");
                }
            }
            else if (ch == ',')
            {
                state = 36;
            }
            else if (ch == ';')
            {
                state = 37;
            }
            else if (ch == '(')
            {
                state = 38;
            }
            else if (ch == ')')
            {
                state = 39;
            }
            else if (ch == '[')
            {
                state = 40;
            }
            else if (ch == ']')
            {
                state = 41;
            }
            else if (ch == '{')
            {
                state = 42;
            }
            else if (ch == '}')
            {
                state = 43;
            }
            else if (ch == '+')
            {
                state = 44;
            }
            else if (ch == '-')
            {
                state = 45;
            }
            else if (ch == '*')
            {
                state = 46;
            }
            else if (ch == '/')
            {
                // printf("STR %s\n", pCrtCh);
                // printf("CHARRRR %c\n", ch);
                char *aux = pCrtCh + 1;
                pCrtCh++;
                if (*aux == '/')
                {
                    printf("LINE COMMNET\n");
                    state = 63;
                    // pCrtCh++;
                    // tk = addTk(NOTEQ, line);
                    // return tk->code;
                }
                else
                {
                    if (*aux == '*')
                    {
                        pCrtCh++;

                        state = 64;
                    }
                    else
                    {
                        state = 47;
                    }

                    // tk = addTk(NOT, line);
                    // return tk->code;
                }
            }
            else if (ch == '.')
            {
                state = 48;
            }
            else if (ch == '&')
            {
                state = 49;
            }
            else if (ch == '|')
            {
                state = 51;
            }
            else if (ch == '!')
            {
                // printf("STR %s\n", pCrtCh);
                // printf("CHARRRR %c\n", ch);
                char *aux = pCrtCh + 1;
                pCrtCh++;
                if (*aux == '=')
                {
                    state = 58;
                    // pCrtCh++;
                    // tk = addTk(NOTEQ, line);
                    // return tk->code;
                }
                else
                {
                    state = 53;
                    // tk = addTk(NOT, line);
                    // return tk->code;
                }
            }
            else if (ch == '=')
            {
                pCrtCh++;
                state = 54;
            }
            else if (ch == '<')
            {
                char *aux = pCrtCh + 1;
                pCrtCh++;
                if (*aux == '=')
                {
                    state = 60;
                }
                else
                {
                    state = 59;
                }
            }
            else if (ch == '>')
            {
                char *aux = pCrtCh + 1;
                pCrtCh++;
                if (*aux == '=')
                {
                    state = 62;
                }
                else
                {
                    state = 61;
                }
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
            else if (ch == 'e' || ch == 'E')
            {
                printf("E read\n");
                pCrtCh++;
                state = 13;
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
            pCrtCh++;
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
            else if (ch == 'e' || ch == 'E')
            {
                printf("E read\n");
                state = 13;
            }
            else
            {
                state = 16;
            }
            break;
        case 13:
            printf("CASE 13\n");
            pCrtCh++;
            if (ch == '-' || ch == '+')
            {
                printf("- or + read\n");
                state = 14;
            }
            else if (isdigit(ch))
            {
                printf("number read\n");
                state = 15;
            }
            break;
        case 14:
            printf("CASE 14\n");
            if (isdigit(ch))
            {
                printf("number read\n");
                state = 15;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character");
            }
            break;
        case 15:
            printf("CASE 15\n");

            if (isdigit(ch))
            {
                printf("is digit\n");
                pCrtCh++;
            }
            else
            {
                state = 16;
            }
            break;
        case 16:
            printf("CASE 16\n");
            tk = addTk(CT_REAL, line);
            tk->r = atof(pStartCh);
            return tk->code;
            break;
        case 18:
            printf("CASE 18\n");
            pCrtCh++;
            printf("%c\n", ch);
            // if (ch != '\'' && ch != '\\')
            // {
            //     pCrtCh++;
            //     // tk = addTk(CT_CHAR, line);
            //     // return tk->code;
            // }
            if (ch == '\'')
            {
                // pCrtCh++;
                printf("END of single quote\n");
                state = 24;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character inside char");
            }
            break;
        case 20:
            if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '\\' || ch == '\0')
            {
                printf("correct char inside char");
                pCrtCh++;
                state = 21;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character inside ESC");
            }
        case 21:
            // pCrtCh++;
            if (ch == '\'')
            {
                printf("END OF SINGEL quote\n");
                state = 24;
            }
            else if (ch == '\"')
            {
                printf("END of quote\n");
                state = 23;
            }
            // else
            // {
            //     tkerr(addTk(END, line), "invalid character inside 21\n");
            // }
            break;
        case 22:
            // printf("%c\n", ch);
            pCrtCh++;
            if (ch != '\"' && ch != '\\')
            {
                // pCrtCh++;
                printf("HEI\n");
            }
            else if (ch == '\"')
            {
                // pCrtCh++;
                printf("END of quote\n");
                state = 23;
            }
            break;
        case 23:
            tk = addTk(CT_STRING, line);
            return tk->code;
        case 24:
            pCrtCh++;
            tk = addTk(CT_CHAR, line);
            return tk->code;
        case 36:
            printf("CASE 36\n");
            pCrtCh++;
            tk = addTk(COMMA, line);
            return tk->code;
        case 37:
            printf("CASE 37\n");
            pCrtCh++;
            tk = addTk(SEMICOLON, line);
            return tk->code;
        case 38:
            printf("CASE 38\n");
            pCrtCh++;
            tk = addTk(LPAR, line);
            return tk->code;
        case 39:
            printf("CASE 39\n");
            pCrtCh++;
            tk = addTk(RPAR, line);
            return tk->code;
        case 40:
            printf("CASE 40\n");
            pCrtCh++;
            tk = addTk(LBRACKET, line);
            return tk->code;
        case 41:
            printf("CASE 41\n");
            pCrtCh++;
            tk = addTk(RBRACKET, line);
            return tk->code;
        case 42:
            printf("CASE 42\n");
            pCrtCh++;
            tk = addTk(LACC, line);
            return tk->code;
        case 43:
            printf("CASE 43\n");
            pCrtCh++;
            tk = addTk(RACC, line);
            return tk->code;
        case 44:
            pCrtCh++;
            tk = addTk(ADD, line);
            return tk->code;
        case 45:
            pCrtCh++;
            tk = addTk(SUB, line);
            return tk->code;
        case 46:
            pCrtCh++;
            tk = addTk(MUL, line);
            return tk->code;
        case 47:
            pCrtCh++;
            tk = addTk(DIV, line);
            return tk->code;
        case 48:
            pCrtCh++;
            tk = addTk(DOT, line);
            return tk->code;
        case 49:
            printf("CASE 49\n");
            printf("char %c\n", ch);
            if (ch == '&')
            {
                pCrtCh++;
                state = 50;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character");
            }
            break;
        case 50:
            printf("CASE 50\n");
            printf("char %c\n", ch);
            pCrtCh++;
            if (ch == '&')
            {
                printf("AND FOUND\n");
                tk = addTk(AND, line);
                return tk->code;
            }
            else
            {

                tkerr(addTk(END, line), "invalid character");
            }
            break;
        case 51:
            if (ch == '|')
            {
                pCrtCh++;
                state = 52;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character");
            }
            break;
        case 52:
            printf("CASE 52\n");
            pCrtCh++;
            if (ch == '|')
            {
                printf("OR FOUND\n");
                tk = addTk(OR, line);
                return tk->code;
            }
            else
            {
                tkerr(addTk(END, line), "invalid character");
            }
            break;
        case 53:
            printf("hhhhhh %s\n", pCrtCh);
            tk = addTk(NOT, line);
            return tk->code;

            break;
        case 54:
            printf("CASE 54\n");
            printf("char before %c\n", ch);
            // pCrtCh++;
            printf("char after %c\n", ch);
            if (ch == '=')
            {
                printf("EQUAL\n");
                state = 56;
            }
            else
            {
                state = 55;
            }
            break;
        case 55:
            printf("CASE 55!!!!!!\n");
            // pCrtCh++;
            tk = addTk(ASSIGN, line);
            return tk->code;
        case 56:
            // pCrtCh++;
            tk = addTk(EQUAL, line);
            return tk->code;
        case 58:
            printf("CASE  58\n");
            pCrtCh++;
            tk = addTk(NOTEQ, line);
            return tk->code;
        case 59:
            tk = addTk(LESS, line);
            return tk->code;
        case 60:
            pCrtCh++;
            tk = addTk(LESSEQ, line);
            return tk->code;
        case 61:
            tk = addTk(GREATER, line);
            return tk->code;
        case 62:
            pCrtCh++;
            tk = addTk(GREATEREQ, line);
            return tk->code;
        case 63:
            pCrtCh++;
            tk = addTk(LINECOMMENT, line);
            return tk->code;
            break;
        case 64:
            printf("CASE 64\n");
            printf("%c\n", ch);
            // pCrtCh++;
            if (ch == '*')
            {
                char *aux = pCrtCh + 1;
                printf("aux %c\n", *aux);
                if (*aux == '/')
                {
                    pCrtCh++;
                    tk = addTk(COMMENT, line);
                    return tk->code;
                }
                else
                {
                    tkerr(addTk(END, line), "invalid comment");
                }
            }
            // else
            // {
            //     pCrtCh++;
            // }
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

    const char input[] = "/**/";
    pCrtCh = input;

    while (*pCrtCh != '\0')
    {
        getNextToken();
    }

    showTokens();

    // done() here i have to free the memory

    return 0;
}
