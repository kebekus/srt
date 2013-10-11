
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "parser.h"
#include "error.h"

namespace srt
{
	struct node *copy_branch(struct tree *dst, struct node *src)
	{
		switch (src->token) {
			case token_x:
				return new_var(dst, token_x);
			case token_y:
				return new_var(dst, token_y);
			case token_z:
				return new_var(dst, token_z);
			case token_a:
				return new_var(dst, token_a);
			case token_num:
				return new_num(dst, src->value);
			case token_pow:
				return new_pow(dst, copy_branch(dst, src->left), src->value);
			case token_mul:
				return new_node(dst, copy_branch(dst, src->left), token_mul, copy_branch(dst, src->right));
			case token_add:
				return new_node(dst, copy_branch(dst, src->left), token_add, copy_branch(dst, src->right));
			case token_sub:
				return new_node(dst, copy_branch(dst, src->left), token_sub, copy_branch(dst, src->right));
			case token_neg:
				return new_node(dst, 0, token_neg, copy_branch(dst, src->right));
			case token_sqrt:
				return new_node(dst, 0, token_sqrt, copy_branch(dst, src->right));
			case token_err:
				return 0;
			default:
				error::set_num(error::unknown_token);
				return 0;
		}
	}


	int copy(struct tree *dst, struct tree *src)
	{
		error::set_num_pos(error::empty_expression, 0);
		reset_tree(dst);
		dst->root = copy_branch(dst, src->root);
		return 0 != dst->root;
	}
}

