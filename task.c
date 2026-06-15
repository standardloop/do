#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <standardloop/logger.h>

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
        free(task);
    }
}
