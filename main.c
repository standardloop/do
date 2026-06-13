#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // getopt

#include "./do.h"

#include <standardloop/logger.h>

const char *message = "User: do [flags...] [task...]\n"
                      "Runs the specified task.\n"
                      "";

int main(int argc, char **argv)
{
    int opt;
    char *do_file_name = NULL;
    char *task_name = NULL;

    // task us combined with namespace for now
    while ((opt = getopt(argc, argv, "f:t:")) != -1)
    {
        switch (opt)
        {
        case '?':
            // fprintf(stderr, "Usage: %s <filename> [-f]\n", argv[0]);
            Log(FATAL, "todo");
            return 1;
        case 'f':
            do_file_name = optarg;
            break;
        case 't':
            task_name = optarg;
            break;
        default:
            abort();
        }
    }

    if (do_file_name == NULL)
    {
        do_file_name = "./main.do";
        // return 1;
    }

    // 3. Validate that the required -t flag was provided
    if (task_name == NULL)
    {
        task_name = "main:main";
    }

    // 4. Check if the user provided extra arguments without any flags
    if (optind < argc)
    {
        fprintf(stderr, "Warning: Ignoring extra arguments starting from '%s'\n", argv[optind]);
    }

    char *buffer = ReadFile(do_file_name);
    // printf("%s", buffer);
    // exit(1);
    InitLogger(StringToLogLevel("ERROR"), JSON_FMT, true, false, true, true);

    // DoLexerDebugTest(buffer, true);
    DoLexer *lexer = DoLexerInit(buffer);
    DoParser *parser = DoParserInit(lexer);
    // FreeParser(parser);
    Do *do_var = ParseDo(parser);
    if (do_var == NULL)
    {
        return EXIT_FAILURE;
    }

    PrintDo(do_var);
    exit(1);
    RunDoTask(do_var, task_name);
    FreeDo(do_var);
    free(buffer);
    return 0;
}
