#include "colors.h"
#include "debugUtils.h"
#include "fileFuncs.h"
#include "Files.h"
#include "latexDumpDiff.h"
#include "mathExpParser.h"
#include "safetyTreeDiff.h"
#include "tree.h"


#include <stdio.h>

FILE* log_file_html = nullptr;

int main () // TODO add parsing of files/flags
{
    int global_code_error = 0;
    int count_dumps       = 0;

    //log_file_html = getFilename(DUMP_FILE); //NOTE теперь пользователь указывает название дамп файла,
                                              // но из за смены логики - во всех функциях теперь не требуется передавать
                                              // указатель на записываемый файл - просто убрать аргументы?
    log_file_html = fopen(GRAPH_DUMP, "w");   // temporary solution
    DEBUG_PRINT("LOG_FILE_HTML [%p]\n\n", log_file_html);

    tree_t* tree = treeInit();
    if (tree == nullptr)
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to creat tree\n" COLOR_RESET);
        return 1;
    }

    ++count_dumps;
    treeDump(tree, __FILE__, __func__, __LINE__, global_code_error, count_dumps, nullptr, "DUMP %d", count_dumps);

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

    startReading(tree);

    ++count_dumps;
    treeDump (tree, __FILE__, __func__, __LINE__, global_code_error, count_dumps, nullptr, "DUMP %d", count_dumps);
    ++count_dumps;
    latexDump(tree, __FILE__, __func__, __LINE__, count_dumps, "LaTeX DUMP %d", count_dumps);

    fclose(log_file_html);
    treeDestroy(tree);

    const char* math_expression = "10*(30+20*10)+13$";
    int result = 10 * (30 + 20 * 10) + 13;
    size_t index = 0;
    printf("result of your math expression is %d " COLOR_BYELLOW "expected %d\n" COLOR_RESET,
            getRuleG(math_expression, &index), result);

    return 0;
}
