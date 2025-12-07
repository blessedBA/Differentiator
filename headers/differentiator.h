#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#define COPY_LEFT   copySubtree(tree, node->left)
#define COPY_RIGHT  copySubtree(tree, node->right)

#define DIFF_LEFT   getDiffNode(tree, diff_tree, node->left,  diff_variable)
#define DIFF_RIGHT  getDiffNode(tree, diff_tree, node->right, diff_variable)

#define ADD_(left_subtree, right_subtree) creatOperatorNode(diff_tree, left_subtree, right_subtree, ADDITION)
#define SUB_(left_subtree, right_subtree) creatOperatorNode(diff_tree, left_subtree, right_subtree, SUBTRACTION)
#define MUL_(left_subtree, right_subtree) creatOperatorNode(diff_tree, left_subtree, right_subtree, MULTIPLICATION)
#define DIV_(left_subtree, right_subtree) creatOperatorNode(diff_tree, left_subtree, right_subtree, DIVISION)

#define SIN_(subtree)                     creatOperatorNode(diff_tree, subtree, nullptr, SIN)
#define COS_(subtree)                     creatOperatorNode(diff_tree, subtree, nullptr, COS)

#define NUM_(value)         creatDiffNode(NUMBER, value_t {.number = value})
#define VAR_(code)          creatDiffNode(VARIABLE, value_t {.index = code})


tree_t* startDiffTree (const tree_t* tree);
node_t* creatDiffNode (type_t type, value_t value);
void symplifyDiffTree(tree_t* diff_tree);


#endif // DIFFERENTIATOR_H
