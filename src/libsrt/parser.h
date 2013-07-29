
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef PARSER_H
#define PARSER_H

enum {
	token_err = 0,
	token_x,
	token_y,
	token_z,
	token_a,
	token_add,
	token_sub,
	token_mul,
	token_div,
	token_neg,
	token_num,
	token_pow,
	token_sqrt,
	token_sin,
	token_cos,
	token_paran,
};

struct parser_node {
	struct parser_node *left;
	struct parser_node *right;
	int token;
	float value;
};

struct parser_tree {
	struct parser_node *begin;
	struct parser_node *end;
	struct parser_node *cur;
	struct parser_node *root;
};

struct parser_node *parser_new_node(struct parser_tree *tree, struct parser_node *left, int token, struct parser_node *right);
struct parser_node *parser_new_pow(struct parser_tree *tree, struct parser_node *left, float exponent);
struct parser_node *parser_new_num(struct parser_tree *tree, float value);
struct parser_node *parser_new_var(struct parser_tree *tree, int token);
int parser_token_prio(int token);
int parser_parse(struct parser_tree *tree, char *str);
struct parser_tree *parser_alloc_tree(int size);
void parser_free_tree(struct parser_tree *tree);
void parser_reset_tree(struct parser_tree *tree);

#endif
