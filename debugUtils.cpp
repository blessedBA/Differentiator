#include "colors.h"
#include "debugUtils.h"
#include "tree.h"

#include <assert.h>
#include <stdio.h>

void printValue (node_t* node, type_t type)
{
    assert(node);

    printf("value [%p]\n", &node->value);
    switch (type)
    {
        case OPERATION:
            printf("value = %s\n", node->value.oper.name);
            break;
        case VARIABLE:
            printf("value = %d\n", node->value.index);
            break;
        case NUMBER:
            printf("value = %lf\n", node->value.number);
            break;
        default:
            fprintf(stderr, COLOR_BRED "invalid type of node in %s:%d\n", __func__, __LINE__);
    }

    return;
}
