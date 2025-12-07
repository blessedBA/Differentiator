#ifndef SAFETY_TREE_DIFF_H
#define SAFETY_TREE_DIFF_H

#include "errors.h"
#include "tree.h"

#include <stdio.h>
#include <stdarg.h>

typedef struct
{
    const char* file_name;
    const char* func_name;
    int line;
} func_data;

bool treeVerify (const tree_t* tree, errorLog* log, const char* file_name, const char* func_name, int line);
void treeDump (FILE* log_file_html, const tree_t* tree, node_t* node, const char* file_name, const char* func_name, int line,
               node_t* deleted_node, const char* reason, ...);


#endif // SAFETY_TREE_DIFF_H
