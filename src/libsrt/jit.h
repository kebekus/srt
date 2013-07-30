
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef JIT_H
#define JIT_H
#include "parser.h"
struct parser_jit {
	void *data;
};
struct parser_jit *parser_alloc_jit(char *bc, int len);
void parser_reset_jit(struct parser_jit *parser_jit);
void parser_jit_build(struct parser_jit *parser_jit, struct parser_tree *tree, const char *name);
void *parser_jit_func(struct parser_jit *parser_jit, const char *name);
void parser_jit_link(struct parser_jit *parser_jit);
void parser_free_jit(struct parser_jit *parser_jit);
#endif
