#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "./do.h"

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

char *parseVars(DoParser *);
char *parseVars(DoParser *parser)
{
    if (parser == NULL)
    {
        Log(FATAL, "parseVars(DoParser *parser), parser is NULL");
        return NULL;
    }
    if (parser->current_token->type != DoTokenVars || parser->peek_token->type != DoTokenOpenCurlyBrace)
    {
        Log(FATAL, "ns vars invalid");
        return NULL;
    }

    nextDoToken(parser);
    if (parser->current_token->type != DoTokenOpenCurlyBrace || parser->peek_token->type != DoTokenString)
    {
        Log(FATAL, "ns vars invalid");
        return NULL;
    }
    nextDoToken(parser);
    if (parser->current_token->type != DoTokenString || parser->peek_token->type != DoTokenCloseCurlyBrace)
    {
        Log(FATAL, "ns vars invalid");
        return NULL;
    }
    char *ns_vars = QuickAllocatedString(parser->current_token->literal);

    nextDoToken(parser);
    if (parser->current_token->type != DoTokenCloseCurlyBrace)
    {
        Log(FATAL, "ns vars invalid");
        return NULL;
    }
    return ns_vars;
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
        else if (parser->current_token->type == DoTokenVars)
        {
            char *ns_vars = parseVars(parser);
            if (ns_vars == NULL)
            {
                Log(FATAL, "ns_vars returned NULL");
                FreeDoNamespace(namespace);
                return NULL;
            }
            else
            {
                Log(TRACE, "parsed a task, adding it to namespace->tasks");
                if (namespace->vars != NULL)
                {
                    Log(FATAL, "Only 1 global vars is allowed");
                    FreeDoNamespace(namespace);
                    return NULL;
                }
                namespace->vars = ns_vars;
                // Log(TRACE, "%d", namespace->tasks->size);
            }
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
                Log(TRACE, "parsed a task, adding it to namespace->tasks");
                DoDynArrayAddLast(namespace->tasks, (DoTask *)task);
                // Log(TRACE, "%d", namespace->tasks->size);
            }
        }
        nextDoToken(parser);
    }

    return namespace;
}

static char *cleanSpacesFromTaskCmds(char *);
static char *cleanSpacesFromTaskCmds(char *task_cmds)
{
    Log(TRACE, "entering cleanSpacesFromTaskCmds");
    if (task_cmds == NULL)
    {
        Log(FATAL, "task_cmds in cleanSpacesFromTaskCmds is NULL!");
        return NULL;
    }
    char *read_ptr = task_cmds;
    char *write_ptr = task_cmds;
    bool at_line_start = true;

    while (*read_ptr != '\0')
    {
        if (at_line_start)
        {
            if (isspace((unsigned char)*read_ptr))
            {
                if (*read_ptr == '\n')
                {
                    *write_ptr = *read_ptr;
                    write_ptr++;
                }
                read_ptr++;
                continue;
            }
            else
            {
                at_line_start = false;
            }
        }

        if (*read_ptr == '\n')
        {
            at_line_start = true;
        }
        *write_ptr = *read_ptr;
        write_ptr++;
        read_ptr++;
    }

    *write_ptr = '\0';

    // printf("%s\n", task_cmds);
    // exit(1);
    return task_cmds;
}

static bool isExpectingOpenBraceNext(enum DoTokenType type)
{
    return type == DoTokenCmds || type == DoTokenVars || type == DoTokenCheck;
}

// static void checkForDuplicateTaskFieldThenSet(DoTask *, enum DoTokenType);
// static void checkForDuplicateTaskFieldThenSet(DoTask *task, enum DoTokenType type)
// {
//     if(task == NULL) {}
// }

// this function is so hard coded it is insane
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
    Log(DEBUG, "%s", task->name);
    while (ALWAYS)
    {
        if (parser->current_token->type == DoTokenEOF || parser->current_token->type == DoTokenIllegal)
        {
            Log(FATAL, "invalid token in dotask");
            break;
        }
        if (isExpectingOpenBraceNext(parser->current_token->type))
        {
            if (parser->peek_token->type != DoTokenOpenCurlyBrace)
            {
                Log(FATAL, "expected open brace { after vars, cmds, and status");
                break;
            }

            else
            {
                // should be cmds, check, or vars
                enum DoTokenType type_hold = parser->current_token->type;
                nextDoToken(parser);
                if (parser->current_token->type != DoTokenOpenCurlyBrace || parser->peek_token->type != DoTokenString)
                {
                    Log(FATAL, "expected string after the { (open curly brace) that comes after vars, cmds, and status");
                    break;
                }
                else
                {
                    nextDoToken(parser);
                    if (parser->current_token->type != DoTokenString || parser->peek_token->type != DoTokenCloseCurlyBrace)
                    {
                        Log(FATAL, "expected close curly brace after string");
                        break;
                    }
                    else
                    {
                        char *task_cmds_check_var_str = QuickAllocatedString(parser->current_token->literal);
                        if (type_hold == DoTokenCmds)
                        {
                            if (task->cmds != NULL)
                            {
                                FreeDoTask(task);
                                Log(FATAL, "Line %d: duplicate task cmds, only one is allowed", parser->current_token->line);
                                return NULL;
                            }
                            char *task_cmds_with_no_extra_white_space = cleanSpacesFromTaskCmds(task_cmds_check_var_str);
                            task->cmds = task_cmds_with_no_extra_white_space;
                        }
                        else if (type_hold == DoTokenCheck)
                        {
                            if (task->check_cmds != NULL)
                            {
                                FreeDoTask(task);
                                Log(FATAL, "Line %d: duplicate task check cmds, only one is allowed", parser->current_token->line);
                                return NULL;
                            }
                            char *task_cmds_with_no_extra_white_space = cleanSpacesFromTaskCmds(task_cmds_check_var_str);
                            task->check_cmds = task_cmds_with_no_extra_white_space;
                        }
                        else if (type_hold == DoTokenVars)
                        {
                            Log(DEBUG, "hi");
                            if (task->vars != NULL)
                            {
                                FreeDoTask(task);
                                Log(FATAL, "Line %d: duplicate task vars, only one is allowed", parser->current_token->line);
                                return NULL;
                            }
                            task->vars = task_cmds_check_var_str;
                        }
                        else
                        {
                            Log(FATAL, "invalid token type inside task");
                            break;
                        }
                        nextDoToken(parser);
                        if (parser->current_token->type == DoTokenCloseCurlyBrace && parser->peek_token->type == DoTokenCloseCurlyBrace)
                        {
                            nextDoToken(parser);
                            // FIXME, need to see if task has enough fields, at minimum needs cmds
                            break;
                        }
                    }
                }
            }
        }
        nextDoToken(parser);
    }

    // FIXME, need to see if task has enough fields, at minimum needs cmds

    if (task->cmds == NULL)
    {
        Log(FATAL, "task %s needs to have a cmds section", task->name);
        FreeDoTask(task);
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
    Do *do_var = InitDo();
    if (do_var == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    nextDoToken(parser);
    while (parser->current_token != NULL && parser->current_token->type != DoTokenEOF)
    {
        // PrintDoToken(parser->current_token, true);
        if (parser->current_token->type == DoTokenInclude)
        {
            Log(FATAL, "parsing includes not supported yet");
        }
        else if (parser->current_token->type == DoTokenNamespace)
        {
            Log(TRACE, "found a DoTokenNamespace to start parsing");
            DoNamespace *namespace = parseDoNamespace(parser);
            if (namespace == NULL)
            {
                Log(FATAL, "namespace is NULL issue with parseDoNamespace(parser);");
            }
            else
            {
                DoDynArrayAddLast(do_var->namespaces, namespace);
                Log(TRACE, "finished parsing namespace adding to Do dyn array");
            }
        }
        nextDoToken(parser);
    }
    // PrintDoToken(parser->current_token, false);
    //  Log(FATAL, "done");

    return do_var;
}

extern DoParser *DoParserInit(DoLexer *lexer)
{
    Log(TRACE, "entering DoParserInit");
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
