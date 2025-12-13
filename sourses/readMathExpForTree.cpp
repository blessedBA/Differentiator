#include "debugUtils.h"
#include "differentiator.h"
#include "fileFuncs.h"
#include "mathExpParser.h"
#include "readMathExpForTree.h"
#include "tree.h"
#include "safetyTreeDiff.h"

#include <assert.h>
#include <stdio.h>

void readFromFile (tree_t* tree, const char* file_name)
{
    assert(file_name);

    mathExp_t math_exp = {};
    math_exp.expression = creatBuffer(file_name);
    math_exp.size_expression = strlen(math_exp.expression);
    math_exp.capacity_expression = math_exp.size_expression + 1;

    DEBUG_PRINT(COLOR_BBLUE "math expression - %s\n" COLOR_RESET, math_exp.expression);
    tree->root = getRuleG(tree, &math_exp);

    assert(file_name);

    return;
}

node_t* getRuleG (tree_t* tree, mathExp_t* math_exp)
{
    if (TREE_VERIFY) return nullptr;
    assert(math_exp); 

    node_t* node    = getRuleE(math_exp);
    size_t position = math_exp->position;
    if (math_exp->expression[position] != '$')
    {
        LOG_ERROR(&global_error_log, ERR_INVALID_END_MATH_EXP,
                  code_ERR_INVALID_END_MATH_EXP, "invalid end of math expression in getRuleG");
        ADD_CONTEXT(&global_error_log);
        return nullptr;
    }
    math_exp->position++;

    if (TREE_VERIFY) return nullptr;
    assert(math_exp);

    return node;
}

node_t* getRuleN(mathExp_t* math_exp)
{
    assert(math_exp);

    double value = 0;
    int fractional_part = 0.0;
    int count_digits_of_fractional_part = 0;

    while ('0' <= math_exp->expression[math_exp->position] && math_exp->expression[math_exp->position] <= '9')
    {
        value = value * 10 + (math_exp->expression[math_exp->position] - '0');
        math_exp->position++;
        if (math_exp->expression[math_exp->position] == '.')
        {
            math_exp->position++;
            while ('0' <= math_exp->expression[math_exp->position] && math_exp->expression[math_exp->position] <= '9')
            {
                ++count_digits_of_fractional_part;
                fractional_part = fractional_part / 10 + (math_exp->expression[math_exp->position] - '0');
                math_exp->position++;
            }
            fractional_part /= 10^count_digits_of_fractional_part;
            value += fractional_part;
        }
    }
    DEBUG_PRINT(COLOR_BCYAN "readed value in getRuleN = %lf\n", value);
    assert(math_exp);

    return creatNode(NUMBER, value_t {.number = value}, nullptr, nullptr);
}

node_t* getRuleE(mathExp_t* math_exp)
{
    assert(math_exp);

    node_t* add_node = getRuleT(math_exp);
    if (!add_node) return nullptr;

    while (math_exp->expression[math_exp->position] == '+' || math_exp->expression[math_exp->position] == '-')
    {
        int operation = math_exp->expression[math_exp->position];
        math_exp->position++;
        node_t* add_node_2 = getRuleT(math_exp);

        if (operation == '+') 
        {
            add_node = creatNode(OPERATION, value_t {.oper.code = ADDITION,
                                                     .oper.name = "+"      }, add_node, add_node_2);
        }
        else
        {
            add_node = creatNode(OPERATION, value_t {.oper.code = SUBTRACTION,
                                                     .oper.name = "-"         }, add_node, add_node_2);
        }
    }

    assert(math_exp);

    return add_node;
}

node_t* getRuleT(mathExp_t* math_exp)
{
    assert(math_exp);

    node_t* mul_node = getRuleP(math_exp);
    if (!mul_node) return nullptr;

    while (math_exp->expression[math_exp->position] == '*' || math_exp->expression[math_exp->position] == '/')
    {
        int operation = math_exp->expression[math_exp->position];
        math_exp->position++;
        node_t* mul_node_2 = getRuleP(math_exp);
        if (!mul_node_2) return nullptr;

        if (operation == '*')
        {
            mul_node = creatNode(OPERATION, value_t {.oper.code = MULTIPLICATION,
                                                     .oper.name = "*"             }, mul_node, mul_node_2);
        }
        else
        {
            mul_node = creatNode(OPERATION, value_t {.oper.code = MULTIPLICATION,
                                                     .oper.name = "/"             }, mul_node, mul_node_2);
        }
    }

    assert(math_exp);

    return mul_node;
}

node_t* getRuleP(mathExp_t* math_exp)
{
    assert(math_exp);

    if (math_exp->expression[math_exp->position] == '(')
    {
        math_exp->position++;
        node_t* add_node = getRuleE(math_exp);
        if (!add_node) return nullptr;

        if (math_exp->expression[math_exp->position] != ')')
        {
            printSyntaxError(math_exp->expression, &math_exp->position);
            return nullptr;
        }
        math_exp->position++;
        return add_node;
    }

    assert(math_exp);

    return getRuleN(math_exp);
}

node_t* getRuleV (tree_t* tree, mathExp_t* math_exp) // get variable
{
    TREE_VERIFY;
    assert(math_exp);

    

    TREE_VERIFY;
    assert(math_exp);

    return nullptr;
}