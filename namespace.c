#include "./do.h"

extern DoDynArray *InitNamespaces()
{
    DoDynArray *namespaces = DoDynArrayInit(DYN_ARR_NAMESPACE, 10);
    return namespaces;
}
extern void AddDoNamespace(DoDynArray *namespaces, DoParser *parser)
{
    if (namespaces == NULL || parser == NULL)
    {
        return;
    }
}

extern void FreeDoNamespace()
{
    return;
}
