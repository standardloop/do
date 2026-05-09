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
        errno = EINVAL;
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
    do_var->namespaces = DoDynArrayInit(DYN_ARR_NAMESPACE, DEFAULT_DO_DYN_ARR_SIZE);
    if (do_var->namespaces == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    return do_var;
}
static DoNamespace *parseNamespace(DoParser *parser)
{
    if (parser == NULL)
    {
        Log(FATAL, "parser sent into parseNamespace is NULL");
        errno = EINVAL;
        return NULL;
    }
    if (parser->current_token->type != DoTokenNamespace)
    {
        Log(FATAL, "parseNamespace current token is incorrect");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenSpace)
    {
        Log(FATAL, "space must come after namespace");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenString)
    {
        Log(FATAL, "namespace name must come after space");
        return NULL;
    }
    char *namespace_name = QuickAllocatedString(parser->current_token->literal);
    DoNamespace *namespace = InitDoNamespace(namespace_name);
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenSpace)
    {
        FreeDoNamespace(namespace);
        Log(FATAL, "space must come after namespace name");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenOpenCurlyBrace)
    {
        FreeDoNamespace(namespace);
        Log(FATAL, "open curly brace expected after space after namespace name");
        return NULL;
    }

    return namespace;
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
            Log(FATAL, "parsing includes not supported yet");
        }
        else if (parser->current_token->type == DoTokenNamespace)
        {
            DoNamespace *namespace = parseNamespace(parser);
            if (namespace == NULL)
            {
                Log(FATAL, "namespace is NULL issue with parseNamespace(parser);");
            }
            else
            {
                DoDynArrayAddLast(do_var->namespaces, namespace);
            }
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
