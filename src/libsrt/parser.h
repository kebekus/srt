
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef PARSER_H
#define PARSER_H

namespace srt
{
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

	struct node {
		struct node *left;
		struct node *right;
		int token;
		float value;
	};

	struct tree {
		struct node *begin;
		struct node *end;
		struct node *cur;
		struct node *root;
	};

	struct node *new_node(struct tree *tree, struct node *left, int token, struct node *right);
	struct node *new_pow(struct tree *tree, struct node *left, float exponent);
	struct node *new_num(struct tree *tree, float value);
	struct node *new_var(struct tree *tree, int token);
	int token_prio(int token);
	int parse(struct tree *tree, const char *str);
	struct tree *alloc_tree(int size);
	void free_tree(struct tree *tree);
	void reset_tree(struct tree *tree);
}

#endif

