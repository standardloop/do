#include <stdio.h>
#include <stdlib.h>

#include "./do.h"

int main()
{
    char *buffer = ReadFile("./main.do");
    // printf("%s", buffer);
    // exit(1);
    DoLexerDebugTest(buffer, true);

    free(buffer);
    return 0;
}
