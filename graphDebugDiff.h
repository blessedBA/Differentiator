#ifndef GRAPH_DEBUG_DIFF_H
#define GRAPH_DEBUG_DIFF_H

#include "tree.h"

#include <stdio.h>

FILE* creatDotFile (const tree_t* tree, int count_log_files, node_t* deleted_node = nullptr); //
void creatLogPicture (FILE* log_file, const tree_t* tree, int count_log_files); //

#endif // GRAPH_DEBUG_DIFF_H
