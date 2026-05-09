#include <stdio.h>
#include <stdlib.h>
#include <standardloop/logger.h>

#include "./do.h"

extern DoNamespace *InitDoNamespace(char *name)
{
    if (name == NULL)
    {
        // errno todo
        Log(FATAL, "name is NULL for InitDoNamespace");
        return NULL;
    }
    DoNamespace *namespace = malloc(sizeof(DoNamespace));
    if (namespace == NULL)
    {
        Log(FATAL, "couldn't allocate memory for namespace");
        return NULL;
    }
    namespace->name = name;

    namespace->tasks = DoDynArrayInit(DYN_ARR_TASK, DEFAULT_DO_DYN_ARR_SIZE);
    if (namespace->tasks == NULL)
    {
        Log(FATAL, "couldn't allocate memory for namespace tasks");
        return NULL;
    }
    return namespace;
}

extern void FreeDoNamespace(DoNamespace *namespace)
{
    if (namespace != NULL)
    {
        if (namespace->name != NULL)
        {
            free(namespace->name);
        }
        if (namespace->tasks != NULL)
        {
            FreeDoDynArray(namespace->tasks);
        }
        free(namespace);
    }
}
