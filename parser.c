#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "./do.h"

#include <standardloop/logger.h>

static void nextDoToken(DoParser *);

static DoTask *parseTask(DoParser *);

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
static DoNamespace *parseDoNamespace(DoParser *parser)
{
    if (parser == NULL)
    {
        Log(FATAL, "parser sent into parseDoNamespace is NULL");
        errno = EINVAL;
        return NULL;
    }
    if (parser->current_token->type != DoTokenNamespace)
    {
        Log(FATAL, "parseDoNamespace current token is incorrect");
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
    namespace->name = namespace_name;
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenOpenCurlyBrace)
    {
        FreeDoNamespace(namespace);
        Log(FATAL, "open curly brace expected after space after namespace name");
        return NULL;
    }

    nextDoToken(parser);
    while (ALWAYS)
    {
        if (parser->current_token->type == DoTokenEOF || parser->current_token->type == DoTokenIllegal || parser->obj_nested == 0)
        {
            break;
        }
        else if (parser->current_token->type == DoTokenTask)
        {

            DoTask *task = parseTask(parser);
            if (task == NULL)
            {
                Log(FATAL, "parseTask returned NULL");
                FreeDoNamespace(namespace);
                return NULL;
            }
            else
            {
                Log(INFO, "parsed a task, adding it to namespace->tasks");
                DoDynArrayAddLast(namespace->tasks, (DoTask *)task);
                Log(INFO, "%d", namespace->tasks->size);
            }
        }
        nextDoToken(parser);
    }

    return namespace;
}

static DoTask *parseTask(DoParser *parser)
{
    if (parser == NULL)
    {
        Log(FATAL, "parseTask(DoParser *parser), parser is NULL");
        return NULL;
    }
    if (parser->current_token->type != DoTokenTask || parser->peek_token->type != DoTokenString)
    {
        Log(FATAL, "(parser->current_token->type != DoTokenTask || parser->peek_token->type != DoTokenString)");
        return NULL;
    }

    nextDoToken(parser);
    if (parser->current_token->type != DoTokenString || parser->peek_token->type != DoTokenOpenCurlyBrace)
    {
        Log(FATAL, "(parser->current_token->type != DoTokenString || parser->peek_token->type != DoTokenOpenCurlyBrace)");
        return NULL;
    }
    char *task_name = QuickAllocatedString(parser->current_token->literal);
    DoTask *task = InitDoTask(task_name);
    if (task == NULL)
    {
        free(task_name);
        Log(FATAL, "task is NULL");
        return NULL;
    }

    nextDoToken(parser);
    if (parser->current_token->type != DoTokenOpenCurlyBrace || parser->peek_token->type != DoTokenCmds)
    {
        FreeDoTask(task);
        Log(FATAL, "(parser->current_token->type != DoTokenOpenCurlyBrace || parser->peek_token->type != DoTokenCmds)");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenCmds || parser->peek_token->type != DoTokenOpenCurlyBrace)
    {
        FreeDoTask(task);
        Log(FATAL, "(error parsing task)");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenOpenCurlyBrace)
    {
        FreeDoTask(task);
        Log(FATAL, "(error parsing task)");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenString || parser->peek_token->type != DoTokenCloseCurlyBrace)
    {
        FreeDoTask(task);
        Log(FATAL, "(error parsing task)");
        return NULL;
    }
    char *task_cmds = QuickAllocatedString(parser->current_token->literal);
    task->cmds = task_cmds;
    // Log(INFO, "%s", task->cmds);
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenCloseCurlyBrace) // || parser->obj_nested != FIXME)
    {
        FreeDoTask(task);
        Log(FATAL, "(error parsing task)");
        return NULL;
    }

    return task;
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
            Log(INFO, "found a DoTokenNamespace to start parsing");
            DoNamespace *namespace = parseDoNamespace(parser);
            if (namespace == NULL)
            {
                Log(FATAL, "namespace is NULL issue with parseDoNamespace(parser);");
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
    Do *do_var = parse(parser);
    FreeDoParser(parser);
    return do_var;
}
