#include "debugUtils.h"
#include "latexDumpDiff.h"
#include "tree.h"
#include "safetyTreeDiff.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static void printStartLatexDump (FILE* log_file_latex, func_data* f_data, const char* reason);
static void printMathExpression (FILE* log_file_latex, const tree_t* tree, node_t* node);
static void closeLatexDump      (FILE* log_file_latex);
static void addExpToLatex       (FILE* log_file_latex, const tree_t* tree, const char* title);

void latexDump (const tree_t* tree, const char* file_name, const char* func_name, int line,
                int count_log_files, const char* reason, ...)
{
    TREE_VERIFY;
    assert(file_name);
    assert(func_name);
    assert(reason);

    FILE* log_file_latex = fopen("latexDump/latexDump.tex", "w"); // TODO rework to argc/argv
    if (log_file_latex == nullptr)
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to open graphDump.html\n" COLOR_RESET);
        return;
    }

    func_data f_data = {file_name, func_name, line};

    char formatted_reason[200] = {};
    va_list args = NULL;
    va_start(args, reason);
    vsnprintf(formatted_reason, sizeof(formatted_reason), reason, args);
    va_end(args);

    printStartLatexDump(log_file_latex, &f_data, formatted_reason);

    addExpToLatex(log_file_latex, tree, "exp1");

    fprintf(log_file_latex, "\n");
    closeLatexDump(log_file_latex);

    TREE_VERIFY;
    assert(file_name);
    assert(func_name);
    assert(reason);

    return;
}

void printStartLatexDump(FILE* log_file_latex, func_data* f_data, const char* reason)
{
    assert(log_file_latex);
    assert(f_data);

    fprintf(log_file_latex, "\\documentclass{article}\n"
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

    fprintf(log_file_latex, "\\title{DUMP \\ Differentiator. Called from %s %s:%d}\n", f_data->func_name,
                                                                                      f_data->file_name,
                                                                                      f_data->line      );
    fprintf(log_file_latex, "\\author{REASON: %s}\n", reason);
    fprintf(log_file_latex, "\\begin{document}\n");


    assert(log_file_latex);
    assert(f_data);

    return;
}

void printMathExpression (FILE* log_file_latex, const tree_t* tree, node_t* node)
{
    TREE_VERIFY;
    assert(log_file_latex);

    if (node == nullptr) return;

    if (node->value.oper.code != DIVISION)
    {
        printMathExpression(log_file_latex, tree, node->left );
        //printMathExpression(log_file_latex, tree, node->right);
    }

    switch (node->type.code_type)
    {
        case NUMBER:
            fprintf(log_file_latex, "%lf", node->value.number);
            break;
        case VARIABLE:
            printf(COLOR_BMAGENTA "VARIABLE ACTIVATED\n" COLOR_RESET);
            fprintf(log_file_latex, "%c", tree->variables[node->value.index].name);
            break;
        case OPERATION:
            if (strlen(node->value.oper.name) != LENGTH_BASIC_OPER)
            {
                fprintf(log_file_latex, "%s(", node->value.oper.name);
            }
            else
            {
                if (node->value.oper.code == DIVISION)
                {
                    fprintf(log_file_latex, "\\frac{");

                    if (node->left) printMathExpression(log_file_latex, tree, node->left);
                    // TODO add processing error when there is no left
                    fprintf(log_file_latex, "}{");

                    if (node->left) printMathExpression(log_file_latex, tree, node->left);
                    // TODO add processing error when there is no right
                    fprintf(log_file_latex, "}");
                }
                if ((node->type.code_type         == DIVISION || node->type.code_type         == MULTIPLICATION) &&
                    (node->right->type.code_type  == ADDITION || node->right->type.code_type  == SUBTRACTION)       )
                {
                    fprintf(log_file_latex, "(");
                }
                fprintf(log_file_latex, "%s", node->value.oper.name);
            }

            if (strlen(node->value.oper.name) != LENGTH_BASIC_OPER) fprintf(log_file_latex, ")");
            break;
    }

    if (node->value.oper.code != DIVISION) printMathExpression(log_file_latex, tree, node->right);

    TREE_VERIFY;
    assert(log_file_latex);

    return;
}

void closeLatexDump(FILE* log_file_latex)
{
    if (log_file_latex)
    {
        fprintf(log_file_latex, "\\end{document}\n");
        if (fclose(log_file_latex) != 0) fprintf(stderr, COLOR_BRED "ERROR: failed to close latex file\n" COLOR_RESET);
        printf(COLOR_BGREEN "LaTeX is done\n" COLOR_RESET);
    }

    return;
}

void addExpToLatex (FILE* log_file_latex, const tree_t* tree, const char* title)
{
    TREE_VERIFY;
    assert(log_file_latex);
    assert(title);

    fprintf(log_file_latex, "\\subsection*{%s}\n", title);
    fprintf(log_file_latex, "\\[\n");

    printMathExpression(log_file_latex, tree,  tree->root);

    fprintf(log_file_latex, "\n\\]\n\n");

    TREE_VERIFY;
    assert(log_file_latex);
    assert(title);

    return;
}
