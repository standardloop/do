#ifndef STANDARDLOOP_DO_H
#define STANDARDLOOP_DO_H

#include <standardloop/util.h>

extern char *ReadFile(char *);

typedef struct
{

} DoFlags;

typedef struct
{

} DoVars;

typedef struct
{

} DoEnv;

typedef struct
{

} DoTask;

typedef struct
{
    int config;
    DoTask *tasks;
    DoFlags *flags;
    DoVars *vars;
    DoEnv *envs;
} DoNamespace;

typedef struct
{
    char *path;
    bool is_remote;
} DoIncludes;

typedef struct
{
    DoIncludes *includes;
    DoNamespace *namespaces;
} Do;

extern void FreeDo(Do *);

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
    bool input_error;
    bool memory_error;
    char *error_message;
    int64_t list_nested;
    int64_t obj_nested;
} DoParser;

extern DoParser *DoParserInit(DoLexer *);
extern void PrintDoParserError(DoParser *);
extern void FreeDoParser(DoParser *);
extern void PrintDoParserErrorLine(DoParser *);
extern Do *ParseDo(DoParser *);

// ————————— PARSER END —————————

#endif
