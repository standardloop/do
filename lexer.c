#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "./do.h"

#include <standardloop/logger.h>

static void advanceChar(DoLexer *);
static void backtrackChar(DoLexer *);
static void skipWhitespace(DoLexer *);
static char *makeStringLiteral(DoLexer *);
static char *makeNumberLiteral(DoLexer *);
static char *makeNULLLiteral(DoLexer *);
static char *makeBoolLiteral(DoLexer *);
typedef struct
{
    const char *literal;
    enum DoTokenType type;
} KEY_WORD;

static const KEY_WORD KEY_WORDS[] = {
    {.literal = "include", .type = DoTokenInclude},
    {.literal = "namespace", .type = DoTokenNamespace},
    {.literal = "status", .type = DoTokenStatus},
    {.literal = "cmds", .type = DoTokenCmds},
    {.literal = "flags", .type = DoTokenFlags},
    {.literal = "deps", .type = DoTokenDeps},
    {.literal = "task", .type = DoTokenTask},
    {.literal = "vars", .type = DoTokenVars},
    {.literal = "env", .type = DoTokenEnv},
};

static const int KEY_WORDS_SIZE = sizeof(KEY_WORDS) / sizeof(KEY_WORDS[0]);

extern char *
ReadFile(char *filename)
{
    FILE *file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
    {
        return NULL;
    }

    fseek(file_ptr, 0, SEEK_END);
    u_int64_t length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(file_ptr);
        errno = ENOMEM;
        return NULL;
    }

    fread(buffer, 1, length, file_ptr);
    fclose(file_ptr);
    buffer[length] = NULL_CHAR;

    // printf("%s\n", buffer);
    return buffer;
}

static void advanceChar(DoLexer *lexer)
{
    if (lexer->read_position >= lexer->input_len)
    {
        lexer->current_char = NULL_CHAR;
    }
    else
    {
        lexer->current_char = lexer->input[lexer->read_position];
    }
    lexer->position = lexer->read_position;
    lexer->read_position++;
}

extern DoLexer *DoLexerInit(char *input)
{
    DoLexer *lexer = malloc(sizeof(DoLexer));
    if (lexer == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    lexer->input = input;
    lexer->input_len = strlen(input);
    lexer->current_char = NULL_CHAR;
    lexer->position = -2;
    lexer->read_position = -1;
    lexer->line = 1;

    advanceChar(lexer);

    return lexer;
}

static void backtrackChar(DoLexer *lexer)
{
    lexer->position -= 2;
    lexer->read_position--;
    lexer->current_char = lexer->input[lexer->read_position];
}

static void skipWhitespace(DoLexer *lexer)
{
    while (lexer->current_char == SPACE_CHAR || lexer->current_char == TAB_CHAR || lexer->current_char == NEWLINE_CHAR || lexer->current_char == CARRIAGE_CHAR)
    {
        if (lexer->current_char == NEWLINE_CHAR)
        {
            lexer->line++;
        }
        advanceChar(lexer);
    }
}

static void copyString(char *, char *, size_t, size_t);

static void copyString(char *src, char *des, size_t len, size_t src_offset)
{
    if (src == NULL || des == NULL || len <= 0)
    {
        errno = EINVAL;
        return;
    }

    char *src_it = src + src_offset;
    size_t size = 0;
    while (size < len)
    {
        *des = *src_it;
        des++;
        src_it++;
        size++;
    }
}

static char *parseLiteralOrKeyword(DoLexer *);

static char *parseLiteralOrKeyword(DoLexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }
    char first_char = lexer->current_char;
    u_int32_t start_position = lexer->position;
    while (ALWAYS)
    {
        // printf("%c", lexer->current_char);
        if (lexer->current_char == NULL_CHAR || lexer->current_char == NEWLINE_CHAR ||
            lexer->current_char == SPACE_CHAR || lexer->current_char == DOUBLE_QUOTES_CHAR ||
            lexer->current_char == '(' || lexer->current_char == ')')
        {
            break;
        }
        advanceChar(lexer);
    }
    u_int32_t literal_size = (lexer->position - start_position) + 1;
    char *literal = malloc(sizeof(char) * literal_size);
    if (literal == NULL)
    {
        return NULL;
    }
    copyString(lexer->input, literal, literal_size, start_position);
    literal[literal_size - 1] = NULL_CHAR;

    backtrackChar(lexer);
    return literal;
}

extern DoToken *DoLex(DoLexer *lexer)
{
    DoToken *token = NULL;
    advanceChar(lexer);
    // skipWhitespace(lexer);
    // printf("%c", lexer->current_char);
    u_int32_t curr_pos = lexer->position;
    if (lexer->current_char == NULL_CHAR)
    {
        token = NewDoToken(DoTokenEOF, curr_pos, lexer->position + 1, lexer->line, "\0");
    }
    else if (lexer->current_char == CURLY_OPEN_CHAR)
    {
        token = NewDoToken(DoTokenOpenCurlyBrace, curr_pos, lexer->position + 1, lexer->line, "{");
    }
    else if (lexer->current_char == CURLY_CLOSE_CHAR)
    {
        token = NewDoToken(DoTokenCloseCurlyBrace, curr_pos, lexer->position + 1, lexer->line, "}");
    }
    else if (lexer->current_char == BRACKET_OPEN_CHAR)
    {
        token = NewDoToken(DoTokenOpenBracket, curr_pos, lexer->position + 1, lexer->line, "[");
    }
    else if (lexer->current_char == BRACKET_CLOSE_CHAR)
    {
        token = NewDoToken(DoTokenCloseBracket, curr_pos, lexer->position + 1, lexer->line, "]");
    }
    else if (lexer->current_char == '(')
    {
        token = NewDoToken(DoTokenOpenParentheses, curr_pos, lexer->position + 1, lexer->line, "(");
    }
    else if (lexer->current_char == ')')
    {
        token = NewDoToken(DoTokenCloseParentheses, curr_pos, lexer->position + 1, lexer->line, ")");
    }
    else if (lexer->current_char == DOUBLE_QUOTES_CHAR)
    {
        token = NewDoToken(DoTokenDoubleQuotes, curr_pos, lexer->position + 1, lexer->line, "\"");
    }
    else if (lexer->current_char == SPACE_CHAR)
    {
        token = NewDoToken(DoTokenSpace, curr_pos, lexer->position + 1, lexer->line, " ");
    }
    else if (lexer->current_char == NEWLINE_CHAR)
    {
        lexer->line++;
        token = NewDoToken(DoTokenNewline, curr_pos, lexer->position + 1, lexer->line, "\n");
    }
    else if (lexer->current_char == TAB_CHAR)
    {
        token = NewDoToken(DoTokenTab, curr_pos, lexer->position + 1, lexer->line, "\t");
    }
    else if (lexer->current_char == COMMA_CHAR)
    {
        token = NewDoToken(DoTokenComma, curr_pos, lexer->position + 1, lexer->line, ",");
    }
    else if (lexer->current_char == EQUAL_CHAR)
    {
        token = NewDoToken(DoTokenEqualsSign, curr_pos, lexer->position + 1, lexer->line, "=");
    }
    // handle any words?
    else
    // else if (isalpha(lexer->current_char))
    {
        char *literal_or_keyword = parseLiteralOrKeyword(lexer);

        if (literal_or_keyword == NULL)
        {
            token = NewDoToken(DoTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            bool is_keyword = false;
            for (int i = 0; i < KEY_WORDS_SIZE; i++)
            {
                if (strcmp(literal_or_keyword, KEY_WORDS[i].literal) == 0)
                {
                    token = NewDoToken(KEY_WORDS[i].type, curr_pos, lexer->position + 1, lexer->line, literal_or_keyword);
                    is_keyword = true;
                    break;
                }
            }
            if (is_keyword && token == NULL)
            {
                token = NewDoToken(DoTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
            }
            else if (!is_keyword)
            {
                token = NewDoToken(DoTokenString, curr_pos, lexer->position + 1, lexer->line, literal_or_keyword);
            }
        }
    }
    // else
    // {
    //     Log(DEBUG, "lexer doesn't know what token");
    // }

    return token;
}

extern DoToken *NewDoToken(enum DoTokenType type, u_int32_t start, u_int32_t end, u_int32_t line_num, char *literal)
{
    DoToken *token = malloc(sizeof(DoToken));
    if (token == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    token->type = type;
    token->start = start;
    token->end = end;
    token->literal = literal;
    token->line = line_num;
    return token;
}

extern void PrintDoToken(DoToken *token, bool print_literal)
{
    if (token == NULL)
    {
        errno = EINVAL;
        Log(DEBUG, "PrintDoToken, input token is NULL");
        return;
    }
    printf("Line: %u Place: %u - %u ", token->line, token->start, token->end);
    switch (token->type)
    {
    case DoTokenEOF:
        printf("DoTokenEOF");
        break;
    case DoTokenInclude:
        printf("DoTokenInclude");
        break;
    case DoTokenOpenCurlyBrace:
        printf("DoTokenOpenCurlyBrace");
        break;
    case DoTokenCloseCurlyBrace:
        printf("DoTokenCloseCurlyBrace");
        break;
    case DoTokenDoubleQuotes:
        printf("DoTokenDoubleQuotes");
        break;
    case DoTokenOpenBracket:
        printf("DoTokenOpenBracket");
        break;
    case DoTokenCloseBracket:
        printf("DoTokenCloseBracket");
        break;
    case DoTokenSemiColon:
        printf("DoTokenSemiColon");
        break;
    case DoTokenEqualsSign:
        printf("DoTokenEqualsSign");
        break;
    case DoTokenString:
        printf("DoTokenString");
        break;
    case DoTokenNumber:
        printf("DoTokenNumber");
        break;
    case DoTokenBool:
        printf("DoTokenBool");
        break;
    case DoTokenNamespace:
        printf("DoTokenNamespace");
        break;
    case DoTokenVars:
        printf("DoTokenVars");
        break;
    case DoTokenEnv:
        printf("DoTokenEnv");
        break;
    case DoTokenTask:
        printf("DoTokenTask");
        break;
    case DoTokenDeps:
        printf("DoTokenDeps");
        break;
    case DoTokenFlags:
        printf("DoTokenFlags");
        break;
    case DoTokenCmds:
        printf("DoTokenCmds");
        break;
    case DoTokenStatus:
        printf("DoTokenStatus");
        break;
    case DoTokenSpace:
        printf("DoTokenSpace");
        break;
    case DoTokenTab:
        printf("DoTokenTab");
        break;
    case DoTokenNewline:
        printf("DoTokenNewline");
        break;
    case DoTokenOpenParentheses:
        printf("DoTokenOpenParentheses");
        break;
    case DoTokenCloseParentheses:
        printf("DoTokenCloseParentheses");
        break;
    case DoTokenComma:
        printf("DoTokenComma");
        break;
    case DoTokenIllegal:
        printf("DoTokenIllegal");
        break;
    }

    if (print_literal && (token->type == DoTokenString))
    {
        printf(" : %s\n", token->literal);
        // if (token->type == DoTokenString)
        // {
        //     printf("Literal: \"%s\"\n", token->literal);
        // }
        // else
        // {
        //     printf("Literal: %s\n", token->literal);
        // }
    }
    else
    {
        printf("\n");
    }
    // printf("\n");
}

extern void DoLexerDebugTest(char *input_str, bool exit_after)
{
    DoLexer *lexer = DoLexerInit(input_str);

    while (ALWAYS)
    {
        DoToken *token = DoLex(lexer);
        PrintDoToken(token, true);
        if (token != NULL)
        {
            if (token->type == DoTokenEOF)
            {
                FreeDoToken(token);
                break;
            }
        }
        // else if (token->type == DoTokenIllegal)
        // {
        //     FreeDoToken(token);
        //     FreeDoLexer(lexer);
        //     printf("Do is invalid!\n");
        //     return NULL;
        // }

        FreeDoToken(token);
    }
    FreeDoLexer(lexer);
    if (exit_after)
    {
        exit(1);
    }
}
extern void FreeDoLexer(DoLexer *lexer)
{
    if (lexer == NULL)
    {
        errno = EINVAL;
        return;
    }

    free(lexer);
}

extern void FreeDoToken(DoToken *token)
{
    if (token == NULL)
    {
        errno = EINVAL;
        return;
    }

    if (token->type == DoTokenInclude || token->type == DoTokenNamespace || token->type == DoTokenStatus ||
        token->type == DoTokenCmds || token->type == DoTokenFlags || token->type == DoTokenDeps ||
        token->type == DoTokenTask || token->type == DoTokenEnv || token->type == DoTokenVars || token->type == DoTokenString)
    {
        if (token->literal != NULL)
        {
            free(token->literal);
        }
    }
    free(token);
}
