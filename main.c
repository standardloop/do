#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // getopt

#include "./do.h"

#include <standardloop/logger.h>

const char *message = "User: do [flags...] [task...]\n"
                      "Runs the specified task.\n"
                      "";

static char *addDefaultNamespaceToTask(char *);

static char *addDefaultNamespaceToTask(char *task_name)
{
    char *return_value = task_name;
    if (!IsCharInString(task_name, COLON_CHAR))
    {
        size_t current_task_name_len = strlen(task_name) + 1;
        const size_t default_task_prefix_len = 5; // "main:" 5 (not including \0);
        char *new_task_name_default_ns = malloc(sizeof(char) * (current_task_name_len + default_task_prefix_len));
        if (new_task_name_default_ns == NULL)
        {
            Log(FATAL, "no memory for new_task_name_default_ns");
        }
        else
        {
            new_task_name_default_ns[0] = 'm';
            new_task_name_default_ns[1] = 'a';
            new_task_name_default_ns[2] = 'i';
            new_task_name_default_ns[3] = 'n';
            new_task_name_default_ns[4] = ':';
            for (size_t i = default_task_prefix_len; i < current_task_name_len + default_task_prefix_len; i++)
            {
                new_task_name_default_ns[i] = task_name[i - default_task_prefix_len];
            }
            return_value = new_task_name_default_ns;
        }
    }
    return return_value;
}

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
    }

    if (task_name == NULL)
    {
        task_name = "main:main";
    }

    if (optind < argc)
    {
        Log(WARN, "Warning: Ignoring extra arguments starting from '%s'\n", argv[optind]);
    }

    char *buffer = ReadFile(do_file_name);
    if (buffer == NULL)
    {
        Log(FATAL, "buffer from reading file %s is NULL", do_file_name);
    }

    InitLogger(StringToLogLevel("ERROR"), JSON_FMT, true, false, true, true);

    // DoLexerDebugTest(buffer, true);
    DoLexer *lexer = DoLexerInit(buffer);
    DoParser *parser = DoParserInit(lexer);

    Do *do_var = ParseDo(parser);
    if (do_var == NULL)
    {
        return EXIT_FAILURE;
    }
    task_name = addDefaultNamespaceToTask(task_name);
    RunDoTask(do_var, task_name);
    FreeDo(do_var);
    free(buffer);
    return 0;
}
