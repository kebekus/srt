
#ifndef COPY_H
#define COPY_H
#include "parser.h"
struct parser_node *parser_copy_branch(struct parser_tree *dst, struct parser_node *src);
int parser_copy(struct parser_tree *dst, struct parser_tree *src);
#endif
