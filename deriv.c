
#include "parser.h"
#include "deriv.h"
#include "error.h"
#include "copy.h"

static struct parser_node *deriv(struct parser_tree *dst, struct parser_node *src, int var)
{
	switch (src->token) {
		case token_x:
			if (token_x == var)
				return parser_new_num(dst, 1);
			else
				return parser_new_num(dst, 0);
		case token_y:
			if (token_y == var)
				return parser_new_num(dst, 1);
			else
				return parser_new_num(dst, 0);
		case token_z:
			if (token_z == var)
				return parser_new_num(dst, 1);
			else
				return parser_new_num(dst, 0);
		case token_a:
			if (token_a == var)
				return parser_new_num(dst, 1);
			else
				return parser_new_num(dst, 0);
		case token_num:
			return parser_new_num(dst, 0);
		case token_pow:
			return parser_new_node(dst,
				deriv(dst, src->left, var),
				token_mul,
				parser_new_node(dst,
					parser_new_num(dst, src->value),
					token_mul,
					parser_new_pow(dst,
						parser_copy_branch(dst, src->left),
						src->value-1
					)
				)
			);
		case token_mul:
			return parser_new_node(dst,
				parser_new_node(dst,
					deriv(dst, src->left, var),
					token_mul,
					parser_copy_branch(dst, src->right)
				),
				token_add,
				parser_new_node(dst,
					parser_copy_branch(dst, src->left),
					token_mul,
					deriv(dst, src->right, var)
				)
			);
		case token_add:
			return parser_new_node(dst,
				deriv(dst, src->left, var),
				token_add,
				deriv(dst, src->right, var)
			);
		case token_sub:
			return parser_new_node(dst,
				deriv(dst, src->left, var),
				token_sub,
				deriv(dst, src->right, var)
			);
		case token_neg:
			return parser_new_node(dst, 0, token_neg, deriv(dst, src->right, var));
		case token_err:
			return 0;
		default:
			set_err_str("unknown token");
			return 0;
	}
}

int parser_deriv(struct parser_tree *dst, struct parser_tree *src, int var)
{
	set_err_str_pos("empty expression", 0);
	parser_reset_tree(dst);
	dst->root = deriv(dst, src->root, var);
	return 0 != dst->root;
}

