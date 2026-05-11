#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "./do.h"
#include <standardloop/logger.h>

static inline bool isDoDynArrayFull(DoDynArray *);
// static inline bool isDoDynArrayEmpty(DoDynArray *);
static void dynamicArrayResize(DoDynArray *);
static void freeDoDynArrayList(enum DoDynArrayTypes, void **, u_int32_t, bool);
static void freeDoDynArrayValue(enum DoDynArrayTypes, void *);

extern DoDynArray *DoDynArrayInit(enum DoDynArrayTypes type, u_int32_t initial_capacity)
{
    DoDynArray *dynamic_array = malloc(sizeof(DoDynArray));
    if (dynamic_array == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    dynamic_array->size = 0;
    dynamic_array->capacity = initial_capacity;
    dynamic_array->list = NULL;
    dynamic_array->type = type;
    if (type == DYN_ARR_TASK)
    {
        dynamic_array->list = malloc(sizeof(DoTask *) * initial_capacity);
    }
    else if (type == DYN_ARR_NAMESPACE)
    {
        dynamic_array->list = malloc(sizeof(DoNamespace *) * initial_capacity);
    }
    else
    {
        Log(ERROR, "dyn arr type not implemented yet");
    }

    if (dynamic_array->list == NULL)
    {
        FreeDoDynArray(dynamic_array);
        errno = ENOMEM;
        return NULL;
    }
    return dynamic_array;
}

extern void DoDynArrayAddFirst(DoDynArray *dynamic_array, void *element)
{
    DoDynArrayAdd(dynamic_array, element, 0);
}

extern void DoDynArrayAddLast(DoDynArray *dynamic_array, void *element)
{
    if (dynamic_array == NULL || element == NULL)
    {
        return;
    }
    DoDynArrayAdd(dynamic_array, element, dynamic_array->size);
}

extern void DoDynArrayAdd(DoDynArray *dynamic_array, void *element, u_int32_t index)
{
    if (dynamic_array == NULL || element == NULL)
    {
        return;
    }
    if (isDoDynArrayFull(dynamic_array))
    {
        dynamicArrayResize(dynamic_array);
    }
    for (u_int32_t i = dynamic_array->size; i > index; i--)
    {
        dynamic_array->list[i] = dynamic_array->list[i - 1];
    }

    dynamic_array->list[index] = element;
    dynamic_array->size++;
}

static void dynamicArrayResize(DoDynArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return;
    }
    void **newList = NULL;

    if (dynamic_array->type == DYN_ARR_TASK)
    {
        newList = malloc(sizeof(DoTask *) * dynamic_array->capacity * DEFAULT_DO_DYN_ARR_RESIZE_MULTIPLE);
    }
    else if (dynamic_array->type == DYN_ARR_NAMESPACE)
    {
        newList = malloc(sizeof(DoNamespace *) * dynamic_array->capacity * DEFAULT_DO_DYN_ARR_RESIZE_MULTIPLE);
    }

    if (newList == NULL)
    {
        // errnomem
        return;
    }

    for (u_int32_t i = 0; i < dynamic_array->size; i++)
    {
        newList[i] = dynamic_array->list[i];
    }
    freeDoDynArrayList(dynamic_array->type, dynamic_array->list, dynamic_array->size, false);
    dynamic_array->list = newList;
    dynamic_array->capacity *= DEFAULT_DO_DYN_ARR_RESIZE_MULTIPLE;
}

static inline bool isDoDynArrayFull(DoDynArray *dynamic_array)
{
    return dynamic_array->capacity == dynamic_array->size;
}

// static inline bool isDoDynArrayEmpty(DoDynArray *dynamic_array)
// {
//     return dynamic_array->size == 0;
// }

static void freeDoDynArrayValue(enum DoDynArrayTypes type, void *item)
{
    if (item == NULL)
    {
        return;
    }
    Log(INFO, "freeDoDynArrayValue: %d", type);
    if (type == DYN_ARR_TASK)
    {
        FreeDoTask((DoTask *)item);
    }
    else if (type == DYN_ARR_NAMESPACE)
    {
        FreeDoNamespace((DoNamespace *)item);
    }
    else
    {
        Log(FATAL, "not implemented yet");
    }
}

static void freeDoDynArrayList(enum DoDynArrayTypes type, void **list, u_int32_t size, bool deep)
{
    Log(INFO, "entering freeDoDynArrayList with type: %d", type);
    if (list == NULL)
    {
        return;
    }
    if (deep)
    {
        for (u_int32_t i = 0; i < size; i++)
        {
            freeDoDynArrayValue(type, list[i]);
        }
    }
    free(list);
}

extern void FreeDoDynArray(DoDynArray *dynamic_array)
{
    Log(INFO, "entering FreeDoDynArray with array type: %d", dynamic_array->type);
    if (dynamic_array != NULL)
    {
        if (dynamic_array->list != NULL)
        {
            freeDoDynArrayList(dynamic_array->type, dynamic_array->list, dynamic_array->size, true);
        }
        free(dynamic_array);
    }
}
