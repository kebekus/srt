
#ifndef JIT_H
#define JIT_H
#include "parser.h"
struct parser_jit {
	void *data;
};
void parser_reset_jit(struct parser_jit *parser_jit);
void parser_jit_build(struct parser_jit *parser_jit, struct parser_tree *tree, char *name);
void *parser_jit_func(struct parser_jit *parser_jit, char *name);
void parser_jit_opt(struct parser_jit *parser_jit);
struct parser_jit *parser_alloc_jit(char *bc, int len);
void parser_free_jit(struct parser_jit *jit);
#endif
