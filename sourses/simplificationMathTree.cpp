#include "../../SquareSolver1/FuncsForRealNum.h"
#include "debugUtils.h"
#include "differentiator.h"
#include "errors.h"
#include "tree.h"
#include "simplificationMathTree.h"
#include "safetyTreeDiff.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


static double tryToCalucateNode  (tree_t* diff_tree, node_t* node);
static double convoliteTwoConst  (tree_t* diff_tree, node_t* node, double left_result,
                                                          double right_result);
static double setResult  (node_t* node, double left_result, double right_result);
static void setChildOfParentNode (node_t* new_node, node_t* parent_node, side_t side);
static void symplifyNode (tree_t* diff_tree, node_t* node, node_t* parent_node);

extern FILE* global_latex_file;
extern int global_count_dumps;
extern FILE* global_log_file_html;

void symplifyDiffTree(tree_t* diff_tree)
{
    assert(diff_tree);
    
    tryToCalucateNode(diff_tree, diff_tree->root);
    
    //symplifyNode(diff_tree, diff_tree->root);
    
    assert(diff_tree);
    
    return;
}

double tryToCalucateNode (tree_t* diff_tree, node_t* node)
{
    assert(diff_tree);
    assert(node);

    double temp_result = BAD_VALUE;
    double temp_result2 = BAD_VALUE;
    switch (node->type.code_type)
    {
        case OPERATION:
            if (node->left)  temp_result  = tryToCalucateNode(diff_tree, node->left);
            if (node->right) temp_result2 = tryToCalucateNode(diff_tree, node->right);

            if (compareDoubleNumbers(temp_result,  BAD_VALUE) ||
                compareDoubleNumbers(temp_result2, BAD_VALUE)   ) return BAD_VALUE;
            else 
            {
                DEBUG_PRINT("now there should reinit subtree with node [%p], temp_result = %lf, temp_result2 = %lf\n",
                            node, temp_result, temp_result2);
                return convoliteTwoConst(diff_tree, node, temp_result, temp_result2);
            }
            break;
        case VARIABLE:
            break;
        case NUMBER:
            DEBUG_PRINT("case " COLOR_BGREEN "NUMBER " COLOR_RESET "activated, value in node = %lf\n", node->value.number);
            temp_result = node->value.number;
            break;
        default:
            LOG_ERROR(&global_error_log, ERR_INVALID_TYPE_NODE, 
                      code_ERR_INVALID_TYPE_NODE, "invalid type of node in tryToCalucateNode");
            ADD_CONTEXT(&global_error_log);
    }
    
    assert(diff_tree);
    assert(node);

    return temp_result;
}

double convoliteTwoConst(tree_t* diff_tree, node_t* node, double left_result,
    double right_result)
{
    assert(diff_tree);
    assert(node);

    double result = setResult(node, left_result, right_result);
    DEBUG_PRINT("deleting node [%p]\n", node);
    node_t* parent_node = node->parent;
    side_t side = (node == node->parent->left) ? LEFT : RIGHT;
    nodeDestroy(diff_tree, node, 1);
    treeDump (global_log_file_html, diff_tree, diff_tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
    node_t* new_node = creatDiffNode(NUMBER, value_t {.number = result});
    new_node->parent = parent_node;
    DEBUG_PRINT("NEW NODE [%p]\n", new_node);
    setChildOfParentNode(new_node, parent_node, side);
    treeDump (global_log_file_html, diff_tree, diff_tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);

    assert(diff_tree);
    assert(node);

    return result;
}

double setResult(node_t* node, double left_result, double right_result)
{
    assert(node);
    DEBUG_PRINT("node in setResult [%p]\n", node);
    DEBUG_PRINT("left_result and right_result in setResult = %lf and %lf\n", left_result, right_result);
    double result = BAD_VALUE;
    switch (node->value.oper.code)
    {
        case ADDITION:
            result = left_result + right_result;
            break;
        case SUBTRACTION:
            result = left_result - right_result;
            break;
        case MULTIPLICATION:
            result = left_result * right_result;
            break;
        case DIVISION:
            if (compareDoubleNumbers(right_result, 0))
            {
                LOG_ERROR(&global_error_log, ERR_DIVISION_BY_ZERO,
                code_ERR_DIVISION_BY_ZERO, "division by zero is undefined");
                ADD_CONTEXT(&global_error_log);
                return result;
            }
            result = left_result / right_result;
            break;
        case SIN:
            result = sin(left_result);
            break;
        case COS:
            result = cos(left_result);
            break;
        default:
            DEBUG_PRINT(COLOR_BRED "DEFAULT IN SETRESULT ACTIVATED\n" COLOR_RESET);
            LOG_ERROR(&global_error_log, ERR_INVALID_CODE_OPER,
            code_ERR_INVALID_CODE_OPER, "invalid code of operation in convoliteTwoConst");
            ADD_CONTEXT(&global_error_log);
            return result;
    }

    assert(node);
    DEBUG_PRINT("result in setResult = %lf\n", result);
    return result;
}

void setChildOfParentNode(node_t* new_node, node_t* parent_node, side_t side)
{
    assert(new_node);

    if (parent_node)
    {
        switch (side)
        {
            case LEFT:
                parent_node->left = new_node;
                break;
            case RIGHT:
                parent_node->right = new_node;
                break;
            default:
                LOG_ERROR(&global_error_log, UNEXPECTED_ERROR,
                code_UNEXPECTED_ERROR, "UNEXPECTED ERROR in symplifyNode");
                ADD_CONTEXT(&global_error_log);
        }
    }

    assert(new_node);

    return;
}

void symplifyNode(tree_t* diff_tree, node_t* node, node_t* parent_node)
{
    assert(diff_tree);
    assert(node);
    assert(parent_node);

    if (node->type.code_type == OPERATION && node->left) symplifyNode(diff_tree, node->left, node);

    if (node->left->type.code_type == NUMBER && node->right->type.code_type == NUMBER)
    {
        double left_value  = node->left->value.number;
        double right_value = node->right->value.number;
        convoliteTwoConst(diff_tree, node, left_value, right_value);
    }

    if (node->type.code_type == OPERATION && node->right) symplifyNode(diff_tree, node->right, node);

    assert(diff_tree);
    assert(node);
    assert(parent_node);
}


