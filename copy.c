
#include "parser.h"
#include "error.h"

struct parser_node *parser_copy_branch(struct parser_tree *dst, struct parser_node *src)
{
	switch (src->token) {
		case token_x:
			return parser_new_var(dst, token_x);
		case token_y:
			return parser_new_var(dst, token_y);
		case token_z:
			return parser_new_var(dst, token_z);
		case token_a:
			return parser_new_var(dst, token_a);
		case token_num:
			return parser_new_num(dst, src->value);
		case token_pow:
			return parser_new_pow(dst, parser_copy_branch(dst, src->left), src->value);
		case token_mul:
			return parser_new_node(dst, parser_copy_branch(dst, src->left), token_mul, parser_copy_branch(dst, src->right));
		case token_add:
			return parser_new_node(dst, parser_copy_branch(dst, src->left), token_add, parser_copy_branch(dst, src->right));
		case token_sub:
			return parser_new_node(dst, parser_copy_branch(dst, src->left), token_sub, parser_copy_branch(dst, src->right));
		case token_neg:
			return parser_new_node(dst, 0, token_neg, parser_copy_branch(dst, src->right));
		case token_sqrt:
			return parser_new_node(dst, 0, token_sqrt, parser_copy_branch(dst, src->right));
		case token_err:
			return 0;
		default:
			set_err_str("unknown token");
			return 0;
	}
}


int parser_copy(struct parser_tree *dst, struct parser_tree *src)
{
	set_err_str_pos("empty expression", 0);
	parser_reset_tree(dst);
	dst->root = parser_copy_branch(dst, src->root);
	return 0 != dst->root;
}

