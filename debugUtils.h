#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include "colors.h"
#include "tree.h"

#ifndef NDEBUG
    #define DEBUG_PRINT(...)              printf(__VA_ARGS__)
    #define DEBUG_PRINT_VALUE(node, type) printValue(node, type)
    #define LOCATION                      COLOR_BYELLOW "in %s %s:%d\n" COLOR_RESET, __func__, __FILE__, __LINE__
    #define DEBUG_PRINT_LOCATION          DEBUG_PRINT(LOCATION)
    #define TREE_VERIFY                   treeVerify(tree, __FILE__, __func__, __LINE__)
#else
    #define DEBUG_PRINT(...)              ((void)0)
    #define DEBUG_PRINT_VALUE             ((void)0)
    #define LOCATION                      ((void)0)
    #define DEBUG_PRINT_LOCATION          ((void)0)
#endif

void printValue (node_t* node, type_t type);


#endif // DEBUG_UTILS_H
