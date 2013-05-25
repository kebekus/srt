
#ifndef REDUCE_H
#define REDUCE_H
#include "parser.h"
int parser_reduce(struct parser_tree *tree);
struct parser_node *parser_reduce_branch(struct parser_node *node);
#endif
