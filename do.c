#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

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
            // printf("namespace: %s, has tasks: ", namespace->name);
            DoDynArray *tasks = namespace->tasks;
            for (u_int32_t j = 0; j < tasks->size; j++)
            {
                DoTask *task = tasks->list[j];
                printf("%s:%s\n", namespace->name, task->name);
                // if (j == tasks->size - 1)
                // {
                //     printf("%s", task->name);
                // }
                // else
                // {
                //     printf("%s, ", task->name);
                // }
            }
            // printf("\n");
        }
    }
}

static StringArr *separateNamespaceAndTask(char *);
static StringArr *separateNamespaceAndTask(char *namespace_colon_task)
{
    StringArr *namespace_colon_task_str_arr = EveryoneExplodeNow(namespace_colon_task, COLON_CHAR);

    if (namespace_colon_task == NULL || namespace_colon_task_str_arr->strings == NULL ||
        namespace_colon_task_str_arr->num_strings != 2 ||
        namespace_colon_task_str_arr->strings[0] == NULL || namespace_colon_task_str_arr->strings[1] == NULL)
    {
        Log(FATAL, "namespace_colon_task");
        FreeStringArr(namespace_colon_task_str_arr);
        return NULL;
    }

    return namespace_colon_task_str_arr;
}

static DoNamespace *findTargetNamespace(Do *, char *);
static DoNamespace *findTargetNamespace(Do *do_var, char *namespace_string)
{
    if (do_var == NULL || namespace_string == NULL)
    {
        Log(FATAL, "do_var == NULL || namespace_string == NULL");
        return NULL;
    }

    DoDynArray *namespace_arr_ptr = do_var->namespaces;
    if (namespace_arr_ptr == NULL || namespace_arr_ptr->type != DYN_ARR_NAMESPACE)
    {
        Log(FATAL, "no do namespaces");
        return NULL;
    }
    bool found_namespace = false;
    DoNamespace *namespace_ptr = NULL;
    for (u_int32_t i = 0; i < namespace_arr_ptr->size; i++)
    {
        namespace_ptr = namespace_arr_ptr->list[i];
        if (strcmp(namespace_ptr->name, namespace_string) == 0)
        {
            // we found the namespace that was asked to be used
            found_namespace = true;
            break;
        }
    }
    if (found_namespace)
    {
        return namespace_ptr;
    }
    return NULL;
}

static DoTask *findTargetTask(DoNamespace *, char *);
static DoTask *findTargetTask(DoNamespace *target_namespace, char *task_string)
{
    if (target_namespace == NULL || task_string == NULL)
    {
        Log(FATAL, "target_namespace == NULL || task_string == NULL");
        return NULL;
    }
    DoDynArray *task_arr_ptr = target_namespace->tasks;
    if (task_arr_ptr == NULL || task_arr_ptr->type != DYN_ARR_TASK)
    {
        Log(FATAL, "no tasks");
        return NULL;
    }
    bool found_task = false;
    DoTask *task_ptr = NULL;
    for (u_int32_t i = 0; i < task_arr_ptr->size; i++)
    {
        task_ptr = task_arr_ptr->list[i];
        if (strcmp(task_ptr->name, task_string) == 0)
        {
            // we found the namespace that was asked to be used
            // Log(FATAL, "pog");
            found_task = true;
            break;
        }
    }
    if (found_task)
    {
        return task_ptr;
    }
    return NULL;
}

// static char **stringArrToExecArgs(StringArr *);
// static char **stringArrToExecArgs(StringArr *arr)
// {
//     if (arr == NULL || arr->num_strings == 0 || arr->strings == NULL)
//     {
//         return NULL;
//     }
//     char **return_value = malloc(sizeof(char *) * (arr->num_strings + 1)); // +1 for NULL at end
//     if (return_value == NULL)
//     {
//         return NULL;
//     }
//     int i;
//     for (i = 0; i < arr->num_strings; i++)
//     {
//         return_value[i] = arr->strings[i];
//     }
//     return_value[i] = NULL;
//     return return_value;
// }

static int runTheTargetTask(char *);

#define CHILD_PID 0
static int runTheTargetTask(char *task)
{
    if (task == NULL)
    {
        Log(FATAL, "ns or task is NULL");
        return 1;
    }
    // PrintBuffer(task->cmds, strlen(task->cmds), true);
    //  exit(0);
    //  // printf("%s\n", task->cmds);
    //  char **exec_args = stringArrToExecArgs(EveryoneExplodeNow(task->cmds, SPACE_CHAR));
    //  if (exec_args == NULL)
    //  {
    //      return -1;
    //  }
    int status;
    pid_t pid = fork();
    if (pid < 0)
    {
        // Error handling for fork failure
        printf("fork failed\n");
        return -1;
    }
    else if (pid == CHILD_PID)
    {
        // ONESHELL will be default
        char *exec_args[] = {"sh", "-c", task, NULL};
        execvp(exec_args[0], exec_args);
        Log(ERROR, "%s\n", strerror(errno));
        exit(errno);
    }
    else
    {
        waitpid(pid, &status, WUNTRACED);

        if (WIFEXITED(status))
        {
            Log(DEBUG, "Child exited normally with status = %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            Log(ERROR, "Child terminated by signal %d\n", WTERMSIG(status));
        }
        else
        {
            Log(ERROR, "Child terminated abnormally for another reason\n");
        }
        return status;
    }
    return 0;
}
static char *addNamespaceVarsToCmds(char *, char *);

static char *concateStringsWithNewlineInMiddle(char *, char *);
static char *concateStringsWithNewlineInMiddle(char *first_str, char *second_str)
{
    size_t first_str_len = strlen(first_str);
    size_t second_str_len = strlen(second_str);

    size_t concate_str_length = first_str_len + second_str_len + 2; // newline and NULL char
    char *concate_str = malloc(sizeof(char) * concate_str_length);
    if (concate_str == NULL)
    {
        Log(ERROR, "no memory for concate_str");
        return NULL;
    }
    // copy first str
    size_t i = 0;
    for (; i < first_str_len; i++)
    {
        concate_str[i] = first_str[i];
    }

    concate_str[i] = NEWLINE_CHAR;
    i++;

    // copy second
    for (size_t t = 0; t < second_str_len && i < concate_str_length; t++)
    {
        concate_str[i] = second_str[t];
        i++;
    }
    concate_str[i] = NULL_CHAR;
    return concate_str;
}

static char *addNamespaceVarsToCmds(char *ns_vars, char *task_cmds)
{
    Log(DEBUG, "entering addNamespaceVarsToCmds");
    if (ns_vars == NULL)
    {
        Log(DEBUG, "ns_vars is NULL");
        return QuickAllocatedString(task_cmds); // duplicate
    }
    if (task_cmds == NULL)
    {
        return NULL;
    }
    char *concate_str = concateStringsWithNewlineInMiddle(ns_vars, task_cmds);

    return concate_str;
}

static char *checkAndAdd(Do *, char *);
// ok
// we need to see if the task cmds contains another task name
static char *checkAndAdd(Do *do_var, char *task_cmds)
{
    // TODO
    // pass taskname in too to make sure a task doesn't call itself
    Log(TRACE, "entering checkAndAdd");
    if (do_var == NULL || task_cmds == NULL)
    {
        Log(FATAL, "checkAndAdd, invalid args");
        return NULL;
    }
    StringArr *split_cmds_by_newline = EveryoneExplodeNow(task_cmds, NEWLINE_CHAR);
    if (split_cmds_by_newline == NULL)
    {
        return NULL;
    }

    char *return_value = QuickAllocatedString("\0");
    char *called_task_cmds_copy = NULL;
    // run through all lines of the cmd
    for (int i = 0; i < split_cmds_by_newline->num_strings; i++)
    {
        char *line_copy = QuickAllocatedString(split_cmds_by_newline->strings[i]);
        StringArr *split_each_line_by_space = EveryoneExplodeNow(split_cmds_by_newline->strings[i], SPACE_CHAR);
        // printf("JOSH %s\n", split_cmds_by_newline->strings[i]);
        // TODO -> check NULL
        char *possible_other_task = split_each_line_by_space->strings[0];
        // run through all namespaces to check if task exists
        for (size_t j = 0; j < do_var->namespaces->size; j++)
        {
            DoNamespace *check_ns = do_var->namespaces->list[j];
            for (size_t k = 0; k < check_ns->tasks->size; k++)
            {
                DoTask *check_task = check_ns->tasks->list[k];
                if (strcmp(possible_other_task, check_task->name) == 0)
                {
                    // need to remove the task name and
                    // add the cmds of the task that was calld
                    // this needs to be done recursivily so that
                    // subsequently called tasks also fill in theres.
                    // printf("pog\n");

                    called_task_cmds_copy = QuickAllocatedString(check_task->cmds);
                    // called_task_cmds_copy = checkAndAdd(do_var, called_task_cmds_copy);
                    break;
                }
                called_task_cmds_copy = NULL;
            }
            if (called_task_cmds_copy != NULL)
            {
                break;
            }
        }
        if (called_task_cmds_copy != NULL)
        {
            return_value = concateStringsWithNewlineInMiddle(return_value, called_task_cmds_copy);
        }
        else
        {
            // printf("josh: %s\n", line_copy);
            // exit(1);
            return_value = concateStringsWithNewlineInMiddle(return_value, line_copy);
        }
    }
    // printf("return_value: %s\n", return_value);
    // exit(1);
    return return_value;
}

extern int RunDoTask(Do *do_var, char *namespace_colon_task)
{
    if (do_var == NULL || namespace_colon_task == NULL)
    {
        Log(FATAL, "do_var == NULL || namespace_colon_task == NULL");
        return 1;
    }
    StringArr *namespace_colon_task_str_arr = separateNamespaceAndTask(namespace_colon_task);

    char *namespace_string = namespace_colon_task_str_arr->strings[0];
    DoNamespace *target_namespace = findTargetNamespace(do_var, namespace_string);

    if (target_namespace == NULL)
    {
        FreeStringArr(namespace_colon_task_str_arr);
        Log(FATAL, "target_namespace== NULL");
        return 1;
    }
    char *task_string = namespace_colon_task_str_arr->strings[1];
    DoTask *target_task = findTargetTask(target_namespace, task_string);
    if (target_task == NULL)
    {
        FreeStringArr(namespace_colon_task_str_arr);
        Log(FATAL, "target_task == NULL");
        return 1;
    }

    // if a task's cmds runs another task, we need to add concate those
    char *full_task_cmds = checkAndAdd(do_var, target_task->cmds);

    char *full_task_with_vars_cmds = addNamespaceVarsToCmds(target_namespace->vars, full_task_cmds);

    // printf("running %s...\n", full_task_with_vars_cmds);
    int task_return_code = runTheTargetTask(full_task_with_vars_cmds);
    free(full_task_with_vars_cmds);

    return task_return_code;
}
