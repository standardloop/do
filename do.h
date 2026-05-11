#ifndef STANDARDLOOP_DO_H
#define STANDARDLOOP_DO_H

#include <standardloop/util.h>

extern char *ReadFile(char *);

#define DEFAULT_DO_DYN_ARR_SIZE 16
#define DEFAULT_DO_DYN_ARR_RESIZE_MULTIPLE 2

enum DoDynArrayTypes
{
    DYN_ARR_TASK,
    DYN_ARR_NAMESPACE,
};

typedef struct
{
    enum DoDynArrayTypes type;
    u_int32_t size;
    u_int32_t capacity;
    void **list;
} DoDynArray;

extern void FreeDoDynArray(DoDynArray *);
extern void DoDynArrayAdd(DoDynArray *, void *, u_int32_t);
extern void DoDynArrayAddLast(DoDynArray *, void *);
extern void DoDynArrayAddFirst(DoDynArray *, void *);
extern DoDynArray *DoDynArrayInit(enum DoDynArrayTypes, u_int32_t);

typedef struct
{

} DoFlags;

typedef struct
{

} DoVars;

typedef struct
{

} DoEnv;

// ————————— LEXER START —————————
enum DoTokenType
{
    DoTokenEOF,
    DoTokenInclude,
    DoTokenOpenCurlyBrace,
    DoTokenCloseCurlyBrace,
    DoTokenDoubleQuotes,
    DoTokenOpenBracket,
    DoTokenCloseBracket,
    DoTokenSemiColon,
    DoTokenEqualsSign,
    DoTokenString,
    DoTokenNumber,
    DoTokenBool,
    DoTokenNamespace,
    DoTokenVars,
    DoTokenEnv,
    DoTokenTask,
    DoTokenDeps,
    DoTokenFlags,
    DoTokenCmds,
    DoTokenStatus,
    DoTokenSpace,
    DoTokenTab,
    DoTokenNewline,
    DoTokenOpenParentheses,
    DoTokenCloseParentheses,
    DoTokenComma,
    DoTokenIllegal,
};

typedef struct
{
    enum DoTokenType type;
    u_int32_t start;
    u_int32_t end;
    u_int32_t line;
    char *literal;
} DoToken;

typedef struct
{
    char *input;
    u_int32_t input_len;
    char current_char;
    u_int32_t position;
    u_int32_t read_position;
    u_int32_t line;
    bool in_command;
} DoLexer;

extern DoLexer *DoLexerInit(char *);
extern void FreeDoLexer(DoLexer *);
extern DoToken *DoLex(DoLexer *);
extern void PrintDoToken(DoToken *, bool);
extern void FreeDoToken(DoToken *);
extern DoToken *NewDoToken(enum DoTokenType, u_int32_t, u_int32_t, u_int32_t, char *);
// extern bool IsDoTokenValueType(DoToken *, bool);

extern void DoLexerDebugTest(char *, bool);

// ————————— LEXER END —————————

// ————————— PARSER START —————————

typedef struct
{
    DoLexer *lexer;
    DoToken *current_token;
    DoToken *peek_token;
    char *error_message;
    int64_t list_nested;
    int64_t obj_nested;
} DoParser;

extern DoParser *DoParserInit(DoLexer *);
extern void PrintDoParserError(DoParser *);
extern void FreeDoParser(DoParser *);
extern void PrintDoParserErrorLine(DoParser *);

typedef struct
{
    char *name;
    // DoDynArray *args;
    char *cmds; // not sure this should be char *
} DoTask;

extern DoTask *InitDoTask(char *);
extern void FreeDoTask(DoTask *);

typedef struct
{
    char *name;
    DoDynArray *tasks;
    // DoFlags *flags;
    // DoVars *vars;
    // DoEnv *envs;
} DoNamespace;

extern DoNamespace *InitDoNamespace(char *);
extern void AddDoNamespace(DoDynArray *, DoParser *);
extern void FreeDoNamespace(DoNamespace *);

// typedef struct
// {
//     char *path;
// } DoIncludes;

typedef struct
{
    // DoIncludes *includes;
    DoDynArray *namespaces;
} Do;

extern Do *ParseDo(DoParser *);

extern void FreeDo(Do *);
// ————————— PARSER END —————————

#endif
