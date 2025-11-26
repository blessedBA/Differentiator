#include "colors.h"
#include "debugUtils.h"
#include "graphDebugDiff.h"
#include "safetyTreeDiff.h"
#include "tree.h"

#include <assert.h>
#include <stdarg.h>

#define NUMBER_ERRORS 4


storageListErrors errors[NUMBER_ERRORS] = { {HAVE_NO_ERRORS,          code_HAVE_NO_ERRORS,          "you have no errors!",      true },
                                            {ERR_FAIL_INIT_TREE,      code_ERR_FAIL_INIT_TREE,      "failed to init tree!",     false},
                                            {ERR_INV_SIZE,            code_ERR_INV_SIZE,            "invalid size of tree",     false},
                                            {ERR_INV_NAME_VAR,        code_ERR_INV_NAME_VAR,        "invalid name of variable", false} };

static void printStartDump    (FILE* log_file_html, func_data* f_data);
static void printMainInfoTree (FILE* log_file_html, const tree_t* tree);
static void printImage        (FILE* log_file_html, int count_log_files);
static void printErrorsInLog  (FILE* log_file_html, int global_code_error);

static int count_log_files = 0;
extern FILE* log_file_html;

void setError (error_t error)
{

    errors[HAVE_NO_ERRORS].isError = false;
    for (int index = 1; index < NUMBER_ERRORS; index++)
    {
        if (errors[index].error == error)
        {
            errors[index].isError = true;
        }
    }

    return;
}

void printErrors()
{
    if (errors[HAVE_NO_ERRORS].isError == false) fprintf(stderr, COLOR_YELLOW "LIST OF ERROR: " COLOR_RESET "\n");
    for (int index = 1; index < NUMBER_ERRORS; index++)
    {
        if (errors[index].isError)
        {
            fprintf(stderr, COLOR_BRED "ERROR: " COLOR_RESET "%s\n", errors[index].description);
        }
    }

    return;
}

#ifndef NDEBUG

isError_t treeVerify (const tree_t* tree, const char* file_name, const char* func_name, int line)
{
    int global_code_error = 0;
    if (tree == nullptr)
    {
        count_log_files++;
        errors[HAVE_NO_ERRORS].isError      = false;
        errors[ERR_FAIL_INIT_TREE].isError  = true;
        global_code_error |= code_ERR_FAIL_INIT_TREE;
        treeDump(tree, file_name, func_name, line, global_code_error, count_log_files, nullptr, "ERROR %d", global_code_error);
        return HAVE_ERROR;
    }
    if (tree->size < 0)
    {
        count_log_files++;
        errors[HAVE_NO_ERRORS].isError = false;
        errors[ERR_INV_SIZE].isError   = true;
        global_code_error |= code_ERR_INV_SIZE;
        treeDump(tree, file_name, func_name, line, global_code_error, count_log_files, nullptr, "ERROR %d", global_code_error);
        return HAVE_ERROR;
    }

    if (errors[HAVE_NO_ERRORS].isError == false)
    {
        treeDump(tree, file_name, func_name, line, global_code_error, count_log_files, nullptr, "ERROR %d", global_code_error);
        return HAVE_ERROR;
    }

    return NO_ERRORS;
}

#else

isError_t treeVerify (tree_t* tree, const char* file_name, const char* func_name, int line)
{
    return NO_ERRORS;
}

#endif

void treeDump (const tree_t* tree, const char* file_name, const char* func_name, int line,
               int global_code_error, int count_log_files, node_t* deleted_node, const char* reason, ...)
{
    // FILE* log_file_html = fopen("graphDumpDiff.html", "a");
    // if (log_file_html == nullptr)
    // {
    //     fprintf(stderr, COLOR_BRED "ERROR: failed to open graphDump.html\n" COLOR_RESET);
    //     return;
    // }

    assert(file_name && func_name);

    func_data f_data = {file_name, func_name, line};

    printStartDump(log_file_html, &f_data);

    char formatted_reason[200] = {};
    va_list args = NULL;
    va_start(args, reason);
    vsnprintf(formatted_reason, sizeof(formatted_reason), reason, args);
    va_end(args);
    fprintf(log_file_html, "<p style=\"color: #0e450cff;\">%s</p>\n", formatted_reason);

    if (tree == nullptr)
    {
        fprintf(log_file_html, "Tree [%p]\n", tree);
        fprintf(log_file_html, "FATAL ERROR\n\n");

        return;
    }
    FILE* log_file = creatDotFile(tree, count_log_files, deleted_node);

    DEBUG_PRINT(COLOR_BCYAN "count_log_files right before creatLogPicture = %d\n" COLOR_RESET, count_log_files);

    if (log_file != nullptr) creatLogPicture(log_file, tree, count_log_files);
    else                     fprintf(stderr, COLOR_BRED "ERROR: failed to open log_file%d.txt\n" COLOR_RESET, count_log_files);

    printErrorsInLog(log_file_html, global_code_error);
    printMainInfoTree(log_file_html, tree);
    printImage(log_file_html, count_log_files);

    fprintf(log_file_html, "-----------------------------------END-TreeDump------------------------\n\n");
    fprintf(log_file_html, "</pre>\n");

    //fclose(log_file_html);

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

void printErrorsInLog (FILE* log_file_html, int global_code_error)
{
    assert(log_file_html);

    if (global_code_error)
    {
        fprintf(log_file_html, COLOR_BRED "error code:" COLOR_BYELLOW "\t%d\n" COLOR_RESET, global_code_error);
        fprintf(log_file_html, "ERRORS:\n");
        for (index_t index = 0; index < NUMBER_ERRORS; index++)
        {
            if (errors[index].isError)
            {
                fprintf(log_file_html, COLOR_BRED "error%d" COLOR_RESET ": %s\n", index, errors[index].description);
            }
        }
    }

    assert(log_file_html);

    return;
}

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

void printImage (FILE* log_file_html, int count_log_files)
{
    assert(log_file_html);

    fprintf(log_file_html, "IMAGE\n");
    fprintf(log_file_html, "<img src=\"./images/logFile_%d.png\" width=\"%d\" alt=\"DUMP %d\"/>\n\n", count_log_files, 1000, count_log_files);

    assert(log_file_html);

    return;
}
