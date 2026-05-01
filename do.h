#ifndef STANDARDLOOP_DO_H
#define STANDARDLOOP_DO_H

#include <standardloop/util.h>

extern char *ReadFile(char *);

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

typedef struct
{
} Do;

#endif
