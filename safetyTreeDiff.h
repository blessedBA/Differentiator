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

void setError (error_t error);
void printErrors();
isError_t treeVerify (const tree_t* tree, const char* file_name, const char* func_name, int line);
void treeDump (FILE* log_file_html, const tree_t* tree, const char* file_name, const char* func_name, int line,
               int global_code_error, int count_log_files, node_t* deleted_node, const char* reason, ...);


#endif // SAFETY_TREE_DIFF_H
