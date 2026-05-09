#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "./do.h"

#include <standardloop/logger.h>

static void nextDoToken(DoParser *);
static void nextDoToken(DoParser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    FreeDoToken(parser->current_token);
    parser->current_token = parser->peek_token;
    if (parser->current_token != NULL)
    {
        if (parser->current_token->type == DoTokenOpenBracket)
        {
            parser->list_nested++;
        }
        else if (parser->current_token->type == DoTokenCloseBracket)
        {
            parser->list_nested--;
        }
        if (parser->current_token->type == DoTokenOpenCurlyBrace)
        {
            parser->obj_nested++;
        }
        else if (parser->current_token->type == DoTokenCloseCurlyBrace)
        {
            parser->obj_nested--;
        }
    }
    parser->peek_token = DoLex(parser->lexer);
}

static Do *initDo();

static Do *initDo()
{
    Do *do_var = malloc(sizeof(Do));
    if (do_var == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    do_var->namespaces = InitNamespaces();
    if (do_var->namespaces == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    return do_var;
}

static Do *parse(DoParser *parser)
{
    if (parser == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    Do *do_var = initDo();
    if (do_var == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    nextDoToken(parser);
    while (parser->current_token != NULL && parser->current_token->type != DoTokenEOF)
    {
        PrintDoToken(parser->current_token, true);
        if (parser->current_token->type == DoTokenInclude)
        {
            // AddDoInclude(do_var->includes, parser);
        }
        else if (parser->current_token->type == DoTokenNamespace)
        {
            // AddDoNamespace(do_var->namespaces, parser);
        }
        nextDoToken(parser);
    }
    PrintDoToken(parser->current_token, false);
    // Log(FATAL, "done");

    return do_var;
}

extern DoParser *DoParserInit(DoLexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }

    DoParser *parser = malloc(sizeof(DoParser));
    if (parser == NULL)
    {
        FreeDoLexer(lexer);
        errno = ENOMEM;
        return NULL;
    }
    parser->error_message = NULL;
    parser->lexer = lexer;
    parser->list_nested = 0;
    parser->obj_nested = 0;
    parser->current_token = NULL;
    parser->peek_token = NULL;

    nextDoToken(parser);

    return parser;
}

extern void PrintDoParserError(DoParser *parser)
{
    if (parser == NULL)
    {
        return;
    }
}

extern void FreeDoParser(DoParser *parser)
{
    if (parser != NULL)
    {
        if (parser->lexer != NULL)
        {
            FreeDoLexer(parser->lexer);
        }
        if (parser->current_token != NULL)
        {
            FreeDoToken(parser->current_token);
        }
        if (parser->peek_token != NULL)
        {
            FreeDoToken(parser->peek_token);
        }
        free(parser);
    }
}

extern void PrintDoParserErrorLine(DoParser *parser)
{
    if (parser == NULL)
    {
        return;
    }
}
extern Do *ParseDo(DoParser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    // if (do_var == NULL)
    // {
    //     FreeDoParser(parser);
    //     errno = ENOMEM;
    //     return NULL;
    // }

    Do *do_var = parse(parser);

    FreeDoParser(parser);
    return do_var;
}
