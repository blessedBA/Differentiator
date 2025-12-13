#include "debugUtils.h"
#include "errors.h"
#include "latexDumpDiff.h"
#include "tree.h"
#include "safetyTreeDiff.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static void printStartLatexDump (FILE* latex_file, func_data* f_data, const char* reason);
static void addExpToLatex       (FILE* latex_file, const tree_t* tree, const char* title);
static void printMathExpression (FILE* latex_file, const tree_t* tree, node_t* node);
static void printNodeToLatex    (FILE* latex_file, const tree_t* tree, node_t* node);
static void closeLatexDump      (FILE* latex_file);
static void setPriority(FILE* latex_file, node_t* node, mode_bracket mode);

void latexDump (FILE* latex_file, const tree_t* tree, const char* file_name, const char* func_name, int line,
                int* count_log_files, const char* reason, ...)
{
    TREE_VERIFY;
    assert(latex_file);
    assert(file_name);
    assert(func_name);
    assert(reason);
    assert(count_log_files);

    func_data f_data = {file_name, func_name, line};

    ++(*count_log_files);

    char formatted_reason[200] = {};
    va_list args = {};
    va_start(args, reason);
    vsnprintf(formatted_reason, sizeof(formatted_reason), reason, args);
    va_end(args);

    printStartLatexDump(latex_file, &f_data, formatted_reason);

    fprintf(latex_file, "latex dump %d\n", *count_log_files);

    addExpToLatex(latex_file, tree, "exp1");

    fprintf(latex_file, "\n");
    closeLatexDump(latex_file);

    TREE_VERIFY;
    assert(latex_file);
    assert(file_name);
    assert(func_name);
    assert(reason);
    assert(count_log_files);

    return;
}

void printStartLatexDump(FILE* latex_file, func_data* f_data, const char* reason)
{
    assert(latex_file);
    assert(f_data);

    fprintf(latex_file, "\\documentclass{article}\n"
                            "\\usepackage{graphicx}\n"
                            "\\usepackage{multirow}\n"
                            "\\usepackage{amsmath}\n"
                            "\\usepackage[utf8]{inputenc}\n"
                            "\\usepackage[T2A]{fontenc}\n"
                            "\\usepackage[russian]{babel}\n"
                            "\\usepackage[\n"
                            "\ta4paper,\n"
                            "\ttop=1.5cm,\n"
                            "\tbottom=1.75cm,\n"
                            "\tleft=1.3cm,\n"
                            "\tright=1.3cm\n"
                            "]{geometry}\n"                );

    fprintf(latex_file, "\\title{DUMP \\ Differentiator. Called from %s %s:%d}\n", f_data->func_name,
                                                                                      f_data->file_name,
                                                                                      f_data->line      );
    fprintf(latex_file, "\\author{REASON: %s}\n", reason);
    fprintf(latex_file, "\\begin{document}\n");


    assert(latex_file);
    assert(f_data);

    return;
}

void printMathExpression (FILE* latex_file, const tree_t* tree, node_t* node)
{
    TREE_VERIFY;
    assert(latex_file);

    if (node == nullptr) return;

    if (node->value.oper.code != DIVISION) printMathExpression(latex_file, tree, node->left );

    printNodeToLatex(latex_file, tree, node);

    if (node->value.oper.code != DIVISION) printMathExpression(latex_file, tree, node->right);

    TREE_VERIFY;
    assert(latex_file);

    return;
}

void closeLatexDump(FILE* latex_file)
{
    assert(latex_file);

    if (latex_file)
    {
        fprintf(latex_file, "\\end{document}\n");
        printf(COLOR_BGREEN "LaTeX is done\n" COLOR_RESET);
    }

    assert(latex_file);

    return;
}

void addExpToLatex (FILE* latex_file, const tree_t* tree, const char* title)
{
    TREE_VERIFY;
    assert(latex_file);
    assert(title);

    fprintf(latex_file, "\\subsection*{%s}\n", title);
    fprintf(latex_file, "\\[\n");

    printMathExpression(latex_file, tree, tree->root);

    fprintf(latex_file, "\n\\]\n\n");

    TREE_VERIFY;
    assert(latex_file);
    assert(title);

    return;
}

void printNodeToLatex (FILE* latex_file, const tree_t* tree, node_t* node)
{
    TREE_VERIFY;
    assert(node);
    assert(latex_file);

    // prior_t parent_priority = node->parent->priority;
    // prior_t curr_priority   = node->priority;

    switch (node->type.code_type)
    {
        case TYPE_NULL:
            fprintf(latex_file, "null_type");
            break;
        case NUMBER:
            setPriority(latex_file, node, BEFORE);
            fprintf(latex_file, "%lf", node->value.number);
            setPriority(latex_file, node, AFTER);
            break;
        case VARIABLE:
            DEBUG_PRINT(COLOR_BMAGENTA "VARIABLE ACTIVATED\n" COLOR_RESET);
            setPriority(latex_file, node, BEFORE);
            fprintf(latex_file, "%c", tree->variables[node->value.index].name);
            setPriority(latex_file, node, AFTER);
            break;
        case OPERATION:
            if (strlen(node->value.oper.name) != LENGTH_BASIC_OPER) 
                fprintf(latex_file, "%s(", node->value.oper.name);
            else
            {
                if (node->value.oper.code == DIVISION)
                {
                    fprintf(latex_file, "\\frac{");

                    if (node->left) printMathExpression(latex_file, tree, node->left);
                    // TODO add processing error when there is no left
                    fprintf(latex_file, "}{");

                    if (node->left) printMathExpression(latex_file, tree, node->left);
                    // TODO add processing error when there is no right
                    fprintf(latex_file, "}");
                }
                fprintf(latex_file, "%s", node->value.oper.name);
            }

            if (strlen(node->value.oper.name) != LENGTH_BASIC_OPER) fprintf(latex_file, ")");
            break;
        default:
            fprintf(stderr, COLOR_BRED "ERROR: invalid type of node in %s/%s:%d\n" COLOR_RESET, __func__, __FILE__, __LINE__);
    }

    TREE_VERIFY;
    assert(node);
    assert(latex_file);

    return;
}

void setPriority(FILE* latex_file, node_t* node, mode_bracket mode)
{
    assert(latex_file);
    assert(node);

    if (!node->parent || !node->parent->parent) return;

    int parent_priority = node->parent->parent->priority;
    int curr_priority   = node->parent->priority;

    if (curr_priority > parent_priority)
    {
        if      (node == node->parent->left && mode  == BEFORE)  fprintf(latex_file, "(");
        else if (node == node->parent->right && mode == AFTER)   fprintf(latex_file, ")");
    }

    assert(latex_file);
    assert(node);

    return;
}