#include "debugUtils.h"
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

void latexDump (FILE* latex_file, const tree_t* tree, const char* file_name, const char* func_name, int line,
                int count_log_files, const char* reason, ...)
{
    TREE_VERIFY;
    assert(latex_file);
    assert(file_name);
    assert(func_name);
    assert(reason);

    func_data f_data = {file_name, func_name, line};

    char formatted_reason[200] = {};
    va_list args = NULL;
    va_start(args, reason);
    vsnprintf(formatted_reason, sizeof(formatted_reason), reason, args);
    va_end(args);

    printStartLatexDump(latex_file, &f_data, formatted_reason);

    addExpToLatex(latex_file, tree, "exp1");

    fprintf(latex_file, "\n");
    closeLatexDump(latex_file);

    TREE_VERIFY;
    assert(latex_file);
    assert(file_name);
    assert(func_name);
    assert(reason);

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

    switch (node->type.code_type)
    {
        case NUMBER:
            fprintf(latex_file, "%lf", node->value.number);
            break;
        case VARIABLE:
            printf(COLOR_BMAGENTA "VARIABLE ACTIVATED\n" COLOR_RESET);
            fprintf(latex_file, "%c", tree->variables[node->value.index].name);
            break;
        case OPERATION:
            if (strlen(node->value.oper.name) != LENGTH_BASIC_OPER)
            {
                fprintf(latex_file, "%s(", node->value.oper.name);
            }
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
                if ((node->value.oper.code         == DIVISION || node->value.oper.code         == MULTIPLICATION) &&
                    (node->right->value.oper.code  == ADDITION || node->right->value.oper.code  == SUBTRACTION)       )
                {
                    fprintf(latex_file, "(");
                }
                fprintf(latex_file, "%s", node->value.oper.name);
            }

            if (strlen(node->value.oper.name) != LENGTH_BASIC_OPER) fprintf(latex_file, ")");
            break;
    }

    TREE_VERIFY;
    assert(node);
    assert(latex_file);

    return;
}
