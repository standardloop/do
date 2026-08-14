#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "./do.h"

extern DoTask *InitDoTask(char *name)
{
    if (name == NULL)
    {
        return NULL;
    }
    DoTask *task = malloc(sizeof(DoTask));
    task->name = name;
    task->cmds = NULL;
    task->check_cmds = NULL;
    task->vars = NULL;
    return task;
}

extern void FreeDoTask(DoTask *task)
{
    if (task != NULL)
    {
        if (task->name != NULL)
        {
            free(task->name);
        }
        if (task->cmds != NULL)
        {
            // FIXME
            free(task->cmds);
        }
        if (task->check_cmds != NULL)
        {
            free(task->check_cmds);
        }
        if (task->vars != NULL)
        {
            free(task->vars);
        }
        free(task);
    }
}

extern void PrintDoTask(DoTask *task)
{
    if (task != NULL)
    {
        if (task->name != NULL)
        {
            printf("task->name = %s\n", task->name);
        }
    }
}
