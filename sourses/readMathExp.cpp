#include "debugUtils.h"
#include "fileFuncs.h"
#include "readMathExp.h"
#include "tree.h"
#include "safetyTreeDiff.h"

#include <assert.h>
#include <stdio.h>

static node_t* getRuleG (tree_t* tree, mathExp_t* math_exp);

tree_t* readFromFile (const char* file_name)
{
    assert(file_name);

    mathExp_t math_exp = {};
    math_exp.expression = creatBuffer(file_name);
    math_exp.size_expression = strlen(math_exp.expression);
    math_exp.capacity_expression = math_exp.size_expression + 1;

    tree_t* tree = treeInit();

    tree->root = getRuleG(tree, &math_exp);

    assert(file_name);

}


node_t* getRuleG (tree_t* tree, mathExp_t* math_exp)
{
    TREE_VERIFY;
    assert(math_exp); 

    node_t* node    = getRuleV(tree, math_exp);
    size_t position = math_exp->position;
    if (math_exp->expression[position] != '$')
    {
        LOG_ERROR(&global_error_log, ERR_INVALID_END_MATH_EXP,
                  code_ERR_INVALID_END_MATH_EXP, "invalid end of math expression in getRuleG");
        ADD_CONTEXT(&global_error_log);
        return nullptr;
    }
    math_exp->position++;

    TREE_VERIFY;
    assert(math_exp);

    return node;
}

node_t* getRuleV (tree_t* tree, mathExp_t* math_exp)
{
    TREE_VERIFY;
    assert(math_exp);

    

    TREE_VERIFY;
    assert(math_exp);

    return nullptr;
}