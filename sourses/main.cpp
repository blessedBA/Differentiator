#include "argParserDiff.h"
#include "colors.h"
#include "debugUtils.h"
#include "differentiator.h"
#include "errors.h"
#include "fileFuncs.h"
#include "Files.h"
#include "latexDumpDiff.h"
#include "mathExpParser.h"
#include "simplificationMathTree.h"
#include "safetyTreeDiff.h"
#include "tree.h"


#include <stdio.h>

#define HAVE_ARG true

FileStorage file_storage = {};

FILE* global_latex_file = nullptr;
FILE* global_log_file_html = nullptr;
int global_count_dumps = 0;
int main (const int argc, char* argv[])
{
    
    initErrorLog(&global_error_log);

    FlagStorage storage = {};

    const ConsoleFlag list_flags[] =
        { {"--input",     "-i",  inputFileEnable,     HAVE_ARG },
          {"--dumpHtml",  "-dH", dumpHtmlFileEnable,  HAVE_ARG },
          {"--dumpLatex", "-dL", dumpLatexFileEnable, HAVE_ARG }  };

    size_t count_flags  = sizeof(list_flags) / sizeof(list_flags[0]);
    getFlags(argc, argv, list_flags, count_flags, &storage);

    if (openFiles(&storage, &file_storage)) return -1;

    FILE* log_file = nullptr;
    setErrorLogFile(&global_error_log, &log_file);

    int count_dumps       = 0;

    tree_t* tree = treeInit();
    if (tree == nullptr)
    {
        LOG_ERROR(&global_error_log, ERR_FAIL_INIT_TREE, code_ERR_FAIL_INIT_TREE, "failed to init tree for differentiator");
        ADD_CONTEXT(&global_error_log);
        return 1;
    }

    ++count_dumps;
    FILE* log_file_html = file_storage.log_file_html.pointer;
    FILE* latex_file    = file_storage.latex_file.pointer;
    global_log_file_html = log_file_html;
    global_latex_file = latex_file;
    global_count_dumps = count_dumps;
    treeDump(log_file_html, tree, tree->root, LOC_CALL, nullptr, "DUMP %d", count_dumps);

    tree->root->type.code_type =  OPERATION;
    tree->root->type.name_type = "OPERATION";

    DEBUG_PRINT("value in root after initialization in main [%p]\n", &tree->root->value);
    tree->root->value.oper.name = "*";
    DEBUG_PRINT("value in root after initialization in main after initialization value as operation [%p]\n", &tree->root->value);

//     double number = 12;
//     char*  string_addition = "+";
//     char*  string_multiplication = "*";
//     char var_x = 'x';
//
//     DEBUG_PRINT(LOCATION);
//
//     DEBUG_PRINT(COLOR_BBLUE "expected initialization OPERATION, value = %s\n" COLOR_RESET, string_multiplication);
//     nodeInit(tree, tree->root, LEFT,  OPERATION,  string_multiplication);
//     DEBUG_PRINT(COLOR_BBLUE "\nafter initialization OPERATION, type = %s,  value = %s\n" COLOR_RESET,
//                             tree->root->left->type.name_type, tree->root->left->value.oper.name);
//
//     DEBUG_PRINT(COLOR_BBLUE "expected initialization OPERATION, value = %s\n" COLOR_RESET, string_addition);
//     nodeInit(tree, tree->root, RIGHT, OPERATION, string_addition);
//     DEBUG_PRINT(COLOR_BBLUE "\nafter initialization OPERATION, type = %s,  value = %s\n" COLOR_RESET,
//                             tree->root->right->type.name_type, tree->root->right->value.oper.name);
//
//     DEBUG_PRINT(COLOR_BBLUE "expected initialization NUMBER, value = %lf\n" COLOR_RESET, number);
//     nodeInit(tree, tree->root->right, LEFT, NUMBER, &number);
//     DEBUG_PRINT(COLOR_BBLUE "\nafter initialization NUMBER, type = %s,  value = %lf\n" COLOR_RESET,
//                             tree->root->right->type.name_type, tree->root->right->value.number);
//
//     DEBUG_PRINT(COLOR_BBLUE "expected initialization VARIABLE, value = %s\n" COLOR_RESET, string_addition);
//     nodeInit(tree, tree->root->right, RIGHT, VARIABLE, &var_x);
//     DEBUG_PRINT(COLOR_BBLUE "\nafter initialization VARIABLE, type = %s,  value = %d\n" COLOR_RESET,
//                             tree->root->right->type.name_type, tree->root->right->value.index);
//
//     char var_y = 'y';
//     number = 4;
//
//     DEBUG_PRINT(COLOR_BBLUE "expected initialization NUMBER, value = %lf\n" COLOR_RESET, number);
//     nodeInit(tree, tree->root->left, LEFT, NUMBER, &number);
//     DEBUG_PRINT(COLOR_BBLUE "\nafter initialization NUMBER, type = %s,  value = %lf\n" COLOR_RESET,
//                             tree->root->left->left->type.name_type, tree->root->left->left->value.number);
//
//     DEBUG_PRINT(COLOR_BBLUE "expected initialization VARIABLE, value = %d\n" COLOR_RESET, 1);
//     nodeInit(tree, tree->root->left, RIGHT, VARIABLE, &var_y);
//     DEBUG_PRINT(COLOR_BBLUE "\nafter initialization VARIABLE, type = %s,  value = %d\n" COLOR_RESET,
//                             tree->root->left->right->type.name_type, tree->root->left->right->value.index);
//
//     ++count_dumps;
//     treeDump(tree, __FILE__, __func__, __LINE__, global_code_error, count_dumps, nullptr, "DUMP %d", count_dumps);
//
//     startWriting(tree);

    startReading(tree, &file_storage);

    treeDump (log_file_html, tree, tree->root, LOC_CALL, nullptr, "DUMP %d", ++count_dumps);

    DEBUG_PRINT(COLOR_BRED "PROCESSING TREE ENDED\n\n\n\n" COLOR_RESET);
    tree_t* diff_tree = startDiffTree(tree);

    treeDump (log_file_html, diff_tree, diff_tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++count_dumps);
    //latexDump(latex_file, diff_tree, LOC_CALL, &count_dumps, "LaTeX DIFF DUMP %d", count_dumps + 1);
    printInOrder(diff_tree, diff_tree->root);
    printf("\n");

    symplifyDiffTree(diff_tree);
    treeDump (log_file_html, diff_tree, diff_tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++count_dumps);
    latexDump(latex_file, diff_tree, LOC_CALL, &count_dumps, "LaTeX DIFF DUMP %d", count_dumps + 1);

    closeFiles(&file_storage);
    freeErrorLog(&global_error_log);
    treeDestroy(tree);


// ----------------------math Parser-----------------------------

    const char* math_expression = "10*(30+20*10)+13$";
    int result = 10 * (30 + 20 * 10) + 13;
    size_t index = 0;
    printf("result of your math expression is %d " COLOR_BYELLOW "expected %d\n" COLOR_RESET,
            getRuleG(math_expression, &index), result);


    return 0;
}
