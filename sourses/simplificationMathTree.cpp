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


static double tryToCalculateNode  (tree_t* diff_tree, node_t* node);
static double convoliteTwoConst  (tree_t* diff_tree, node_t* node, double left_result,
                                                          double right_result);
static double setResult  (node_t* node, double left_result, double right_result);
static void setChildOfParentNode (node_t* new_node, node_t* parent_node, side_t side);
//static void symplifyNode (tree_t* diff_tree, node_t* node, node_t* parent_node);

static void simplifyNeutralElements  (tree_t* tree, node_t* node);
static void convoliteZEROAndVariable (tree_t* tree, node_t* node, oper_t type_operation, int index_variable);
static void convoliteNode (tree_t* tree, node_t* node, oper_t code_operation, int index_variable);
static void nodeDestroyWithoutPartOfSubtree (tree_t* tree, node_t* node, side_t side_non_deleting);

extern FILE* global_latex_file;
extern int global_count_dumps;
extern FILE* global_log_file_html;

void symplifyDiffTree(tree_t* diff_tree)
{
    assert(diff_tree);
    
    tryToCalculateNode(diff_tree, diff_tree->root);

    DEBUG_PRINT(COLOR_BCYAN "\n\n\n\n STARTING SIMPLIFYING NEUTRAL ELEMENTS\n\n" COLOR_RESET);

    simplifyNeutralElements(diff_tree, diff_tree->root);
    //symplifyNode(diff_tree, diff_tree->root);
    
    assert(diff_tree);
    
    return;
}

double tryToCalculateNode (tree_t* diff_tree, node_t* node)
{
    assert(diff_tree);
    assert(node);

    double temp_result = BAD_VALUE;
    double temp_result2 = BAD_VALUE;
    switch (node->type.code_type)
    {
        case TYPE_NULL:
            LOG_ERROR(&global_error_log, ERR_INVALID_TYPE_NODE,
                        code_ERR_INVALID_TYPE_NODE, "invalid type of node in tryToCalculateNode");
            ADD_CONTEXT(&global_error_log);
            break;
        case OPERATION:
            if (node->left)  temp_result  = tryToCalculateNode(diff_tree, node->left);
            if (node->right) temp_result2 = tryToCalculateNode(diff_tree, node->right);

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
                      code_ERR_INVALID_TYPE_NODE, "invalid type of node in tryToCalculateNode");
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

    double result = setResult(node, left_result, right_result); // foldOperation
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
        case NULL_OPER:
            break;
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

/* OLD FUNCTION
// void symplifyNode(tree_t* diff_tree, node_t* node, node_t* parent_node)
// {
//     assert(diff_tree);
//     assert(node);
//     assert(parent_node);

//     if (node->type.code_type == OPERATION && node->left) symplifyNode(diff_tree, node->left, node);

//     if (node->left->type.code_type == NUMBER && node->right->type.code_type == NUMBER)
//     {
//         double left_value  = node->left->value.number;
//         double right_value = node->right->value.number;
//         convoliteTwoConst(diff_tree, node, left_value, right_value);
//     }

//     if (node->type.code_type == OPERATION && node->right) symplifyNode(diff_tree, node->right, node);

//     assert(diff_tree);
//     assert(node);
//     assert(parent_node);
// }
*/

void simplifyNeutralElements(tree_t* tree, node_t* node)
{
    if (TREE_VERIFY) return;
    assert(node);

    if (node->left)  simplifyNeutralElements(tree, node->left);
    if (node->right) simplifyNeutralElements(tree, node->right);
    if (!node->left && !node->right) return;

    switch (node->type.code_type)
    {
        case NUMBER:
            return;
        case VARIABLE:
            return;
        case OPERATION:
        {
            if (node->right->type.code_type == NUMBER &&
                node->value.oper.code == DIVISION &&
                compareDoubleNumbers(node->right->value.number, 0))
            {
                LOG_ERROR(&global_error_log, ERR_DIVISION_BY_ZERO,
                            code_ERR_DIVISION_BY_ZERO, "division by zero is undefined in simplifyNeutralElements");
                ADD_CONTEXT(&global_error_log);
                return;
            }

            if (node->left->type.code_type  == NUMBER)
            {
                if (compareDoubleNumbers(node->left->value.number, 0))
                {
                    int var_index = (node->right && node->right->type.code_type == VARIABLE) ?
                                                                    node->right->value.index : 0;
                    convoliteZEROAndVariable(tree, node, node->value.oper.code, var_index);
                    return;
                }
            }
            if (node->right->type.code_type == NUMBER)
            {
                if (compareDoubleNumbers(node->right->value.number, 0))
                { 
                    int var_index = (node->left && node->left->type.code_type == VARIABLE) ?
                                                                    node->left->value.index : 0;
                    convoliteZEROAndVariable(tree, node, node->value.oper.code, var_index);
                    return;
                }
            }
        }
    }

    if (TREE_VERIFY) return;
    assert(node);

    return;
}

void convoliteZEROAndVariable (tree_t* tree, node_t* node, oper_t type_operation, int index_variable)
{
    if (TREE_VERIFY) return;
    assert(node);

    switch (type_operation)
    {
        case ADDITION:
        case SUBTRACTION:
            convoliteNode(tree, node, ADDITION, index_variable); // TODO add processing case where another child is operation 
            break;
        case MULTIPLICATION:
        case DIVISION:
            convoliteNode(tree, node, MULTIPLICATION, index_variable);
            break;
        default:
            break;
    }

    if (TREE_VERIFY) return;
    assert(node);

    return;
}

void convoliteNode (tree_t* tree, node_t* node, oper_t code_operation, int index_variable)
{
    if (TREE_VERIFY) return;
    assert(node);

    DEBUG_PRINT("deleting node [%p]\n", node);
    node_t* parent_node = node->parent;
    side_t side = LEFT;
    if (parent_node) side = (node == node->parent->left) ? LEFT : RIGHT;

    node_t* new_node = nullptr;

    switch (code_operation)
    {
        case ADDITION:
        case SUBTRACTION:
            if (node->right->type.code_type == OPERATION)
            {
                DEBUG_PRINT(COLOR_BYELLOW "nodeDestroyWithoutPartOfSubtree activated for RIGHT subtree of node\n" COLOR_RESET);
                nodeDestroyWithoutPartOfSubtree(tree, node, RIGHT);
                treeDump (global_log_file_html, tree, tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
                return;
            }
            if (node->left->type.code_type  == OPERATION)
            {
                DEBUG_PRINT(COLOR_BYELLOW "nodeDestroyWithoutPartOfSubtree activated for LEFT subtree of node\n" COLOR_RESET);
                nodeDestroyWithoutPartOfSubtree(tree, node, LEFT);
                treeDump (global_log_file_html, tree, tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
                return;
            }
            
            new_node = creatDiffNode(VARIABLE, value_t {.index = index_variable});
            break;
        case MULTIPLICATION:
        case DIVISION:
            nodeDestroy(tree, node, 1);
            new_node = creatDiffNode(NUMBER, value_t {.number = 0.0});
            break;
        default:
            break;
    }
    treeDump (global_log_file_html, tree, tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
    
    new_node->parent = parent_node;
    DEBUG_PRINT("NEW NODE [%p]\n", new_node);
    setChildOfParentNode(new_node, parent_node, side);
    treeDump (global_log_file_html, tree, tree->root, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
    DEBUG_PRINT("global_count_dumps right after treeDump in the end of func convoliteNode = %d", global_count_dumps);

    if (TREE_VERIFY) return;
    assert(node);

    return;
}

void nodeDestroyWithoutPartOfSubtree (tree_t* tree, node_t* node, side_t side_non_deleting)
{
    if (TREE_VERIFY) return;
    assert(node);

    node_t* non_deleted_subtree = nullptr;
    node_t* subtree_to_delete   = nullptr;

    switch (side_non_deleting)
    {
        case WRONG_SIDE:
            LOG_ERROR(&global_error_log, ERR_INVALID_SIDE_NODE,
                      code_ERR_INVALID_SIDE_NODE, "invalid side of non deleting node in nodeDestroyWithoutPartOfSubtree");
            ADD_CONTEXT(&global_error_log);
            return;
        case LEFT:
            non_deleted_subtree = node->left;
            subtree_to_delete   = node->right;
            break;
        case RIGHT:
            non_deleted_subtree = node->right;
            subtree_to_delete   = node->left;
            break;
        default:
            LOG_ERROR(&global_error_log, ERR_UNEXPECTED_ENUM_VALUE,
                      code_ERR_UNEXPECTED_ENUM_VALUE, "unexpected enum constant in nodeDestroyWithoutPartOfSubtree");
            ADD_CONTEXT(&global_error_log);
    }

    non_deleted_subtree->parent = node->parent;
    nodeDestroy(tree, subtree_to_delete, 1);

    if (node->parent)
    {   
        side_t side = getSideOfNodeInParentNode(tree, node);
        switch (side)
        {
            case WRONG_SIDE:
                return;
            case LEFT:
                node->parent->left  = non_deleted_subtree;
                break;
            case RIGHT:
                node->parent->right = non_deleted_subtree;
                break;
        }
        deleteNodeWithoutSubtree(tree, node);
    }
    else // case when node is root of tree
    {
        if (tree->root != node)
        {
            LOG_ERROR(&global_error_log, ERR_NO_MATCH_PARENT_AND_CHILD,
                      code_ERR_NO_MATCH_PARENT_AND_CHILD, "invalid parent of node (nullptr) in nodeDestroyWithoutPartOfSubtree");
            ADD_CONTEXT(&global_error_log);
            return;
        }

        node_t* temp_root = nullptr;
        temp_root  = tree->root;
        tree->root = non_deleted_subtree;
        deleteNodeWithoutSubtree(tree, temp_root);
    }

    TREE_VERIFY;
    assert(node);

    return;
}