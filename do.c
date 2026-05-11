#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "./do.h"

#include <standardloop/logger.h>

extern Do *InitDo()
{
    Do *do_var = malloc(sizeof(Do));
    if (do_var == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    do_var->namespaces = DoDynArrayInit(DYN_ARR_NAMESPACE, DEFAULT_DO_DYN_ARR_SIZE);
    if (do_var->namespaces == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    return do_var;
}

extern void FreeDo(Do *do_var)
{
    if (do_var != NULL)
    {
        if (do_var->namespaces != NULL)
        {
            Log(INFO, "freeing namespace from do with type: %d", do_var->namespaces->type);
            FreeDoDynArray(do_var->namespaces);
        }
        free(do_var);
    }
}

extern void PrintDo(Do *do_var)
{
    if (do_var != NULL)
    {
        for (u_int32_t i = 0; i < do_var->namespaces->size; i++)
        {
            DoNamespace *namespace = do_var->namespaces->list[i];
            printf("namespace: %s, has tasks: ", namespace->name);
            DoDynArray *tasks = namespace->tasks;
            for (u_int32_t j = 0; j < tasks->size; j++)
            {
                DoTask *task = tasks->list[j];
                if (j == tasks->size - 1)
                {
                    printf("%s", task->name);
                }
                else
                {
                    printf("%s, ", task->name);
                }
            }
            printf("\n");
        }
    }
}