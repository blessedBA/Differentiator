#include "colors.h"
#include "debugUtils.h"
#include "errors.h"
#include "graphDebugDiff.h"
#include "fileFuncs.h"
#include "safetyTreeDiff.h"
#include "tree.h"

#include <assert.h>
#include <stdarg.h>

#define NUMBER_ERRORS 4


// storageErrors errors[NUMBER_ERRORS] = { {HAVE_NO_ERRORS,          code_HAVE_NO_ERRORS,          "you have no errors!",      true },
//                                             {ERR_FAIL_INIT_TREE,      code_ERR_FAIL_INIT_TREE,      "failed to init tree!",     false},
//                                             {ERR_INV_SIZE,            code_ERR_INV_SIZE,            "invalid size of tree",     false},
//                                             {ERR_INV_NAME_VAR,        code_ERR_INV_NAME_VAR,        "invalid name of variable", false} };

static void printStartDump    (FILE* log_file_html, func_data* f_data);
static void printMainInfoTree (FILE* log_file_html, const tree_t* tree);
static void printImage        (FILE* log_file_html);
//static void printErrorsInLog  (FILE* log_file_html, int global_code_error);

static int count_log_files = 0;
extern FileStorage file_storage;

#ifndef NDEBUG

bool treeVerify (const tree_t* tree, errorLog* log, const char* file_name, const char* func_name, int line)
{
    bool isError = false;
    if (tree == nullptr)
    {
        LOG_ERROR(&global_error_log, ERR_FAIL_INIT_TREE, code_ERR_FAIL_INIT_TREE, "fail to init tree");
        ADD_CONTEXT(&global_error_log);
        isError = true;
        treeDump(file_storage.log_file_html.pointer, tree, tree->root, file_name, func_name, line, nullptr, "ERROR %d", ++count_log_files);
        return true;
    }
    if (tree->size < 0) isError = true;
    if (log->size  > 0) isError = true;
    
    if (isError) treeDump(file_storage.log_file_html.pointer, tree, tree->root, file_name, func_name, line, nullptr, "ERROR %d", ++count_log_files);

    return (isError) ? true : false;
}

#else

bool treeVerify (const tree_t* tree, const char* file_name, const char* func_name, int line)
{
    return false;
}

#endif

void treeDump (FILE* log_file_html, const tree_t* tree, node_t* node, const char* file_name, const char* func_name, int line,
               node_t* deleted_node, const char* reason, ...)
{
    assert(file_name);
    assert(func_name);
    assert(node);
    assert(log_file_html);

    count_log_files++;

    func_data f_data = {file_name, func_name, line};

    printStartDump(log_file_html, &f_data);

    char formatted_reason[200] = {};
    va_list args = {};
    va_start(args, reason);
    vsnprintf(formatted_reason, sizeof(formatted_reason), reason, args);
    va_end(args);

    fprintf(log_file_html, "<p style=\"color: #0e450cff;\">%s</p>\n", formatted_reason);

    printErrors(&global_error_log);

    if (tree == nullptr)
    {
        fprintf(log_file_html, "Tree [%p]\n", tree);
        fprintf(log_file_html, "FATAL ERROR\n\n");

        return;
    }
    FILE* log_file = creatDotFile(tree, node, count_log_files, deleted_node);

    DEBUG_PRINT(COLOR_BCYAN "count_log_files right before creatLogPicture = %d\n" COLOR_RESET, count_log_files);

    if (log_file != nullptr) creatLogPicture(log_file, tree, count_log_files);
    else                     fprintf(stderr, COLOR_BRED "ERROR: failed to open log_file%d.txt\n" COLOR_RESET, count_log_files);

    printMainInfoTree(log_file_html, tree);
    printImage(log_file_html);

    fprintf(log_file_html, "-----------------------------------END-TreeDump------------------------\n\n");
    fprintf(log_file_html, "</pre>\n");

    assert(file_name);
    assert(func_name);
    assert(node);
    assert(log_file_html);
    
    return;
}

void printStartDump (FILE* log_file_html, func_data* f_data)
{
    assert(log_file_html && f_data);

    fprintf(log_file_html, "<!DOCTYPE html>\n");
    fprintf(log_file_html, "<html lang = 'ru'>\n");
    fprintf(log_file_html, "<head>\n"  );
    fprintf(log_file_html, "<style>\n" );
    fprintf(log_file_html, "body { background-color: #a8cfc3ff }\n");
    fprintf(log_file_html, "</style>\n");
    fprintf(log_file_html, "</head>\n" );
    fprintf(log_file_html, "<body>\n\n");
    fprintf(log_file_html, "<h3 align=\"center\"> TreeDump called from %s:%d from func %s</h3>\n", f_data->file_name, f_data->line, f_data->func_name);
    fprintf(log_file_html, "<pre>\n");
    fprintf(log_file_html, "DUMP NUMBER %d\n", count_log_files);
    fprintf(log_file_html, "-----------------------------------TreeDump------------------------\n\n");

    assert(log_file_html && f_data);

    return;
}

// void printErrorsInLog (FILE* log_file_html, int global_code_error)
// {
//     assert(log_file_html);
//
//     if (global_code_error)
//     {
//         fprintf(log_file_html, COLOR_BRED "error code:" COLOR_BYELLOW "\t%d\n" COLOR_RESET, global_code_error);
//         fprintf(log_file_html, "ERRORS:\n");
//         for (index_t index = 0; index < NUMBER_ERRORS; index++)
//         {
//             if (errors[index].isError)
//             {
//                 fprintf(log_file_html, COLOR_BRED "error%d" COLOR_RESET ": %s\n", index, errors[index].description);
//             }
//         }
//     }
//
//     assert(log_file_html);
//
//     return;
// }

void printMainInfoTree (FILE* log_file_html, const tree_t* tree)
{
    assert(log_file_html && tree);

    fprintf(log_file_html, "tree [%p]:\n", tree);
    fprintf(log_file_html, "size = %zu\n", tree->size);
    fprintf(log_file_html, "variables:\n{\n");

    for (int index = 0; index < NUMBER_VARIABLES; index++)
    {
        fprintf(log_file_html, "\t%d ('%c') = %lf\n", index, tree->variables[index].name, tree->variables[index].value);
    }

    return;
}

void printImage (FILE* log_file_html)
{
    assert(log_file_html);

    fprintf(log_file_html, "IMAGE\n");
    fprintf(log_file_html, "<img src=\"./images/logFile_%d.png\" width=\"%d\" alt=\"DUMP %d\"/>\n\n", count_log_files, 1000, count_log_files);

    assert(log_file_html);

    return;
}
