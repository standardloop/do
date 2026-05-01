#include <stdio.h>
#include <stdlib.h>

#include "./do.h"

#include <standardloop/logger.h>

int main()
{
    char *buffer = ReadFile("./main.do");
    // printf("%s", buffer);
    // exit(1);
    InitLoggerEasy(StringToLogLevel("TRACE"));

    // DoLexerDebugTest(buffer, true);
    DoLexer *lexer = DoLexerInit(buffer);
    DoParser *parser = DoParserInit(lexer);
    Do *do_var = ParseDo(parser);

    // free(buffer);
    return 0;
}
