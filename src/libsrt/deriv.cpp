
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "parser.h"
#include "deriv.h"
#include "error.h"
#include "copy.h"

namespace srt
{
	static struct node *deriv(struct tree *dst, struct node *src, int var)
	{
		switch (src->token) {
			case token_x:
				if (token_x == var)
					return new_num(dst, 1);
				else
					return new_num(dst, 0);
			case token_y:
				if (token_y == var)
					return new_num(dst, 1);
				else
					return new_num(dst, 0);
			case token_z:
				if (token_z == var)
					return new_num(dst, 1);
				else
					return new_num(dst, 0);
			case token_a:
				if (token_a == var)
					return new_num(dst, 1);
				else
					return new_num(dst, 0);
			case token_num:
				return new_num(dst, 0);
			case token_pow:
				return new_node(dst,
					deriv(dst, src->left, var),
					token_mul,
					new_node(dst,
						new_num(dst, src->value),
						token_mul,
						new_pow(dst,
							copy_branch(dst, src->left),
							src->value-1
						)
					)
				);
			case token_mul:
				return new_node(dst,
					new_node(dst,
						deriv(dst, src->left, var),
						token_mul,
						copy_branch(dst, src->right)
					),
					token_add,
					new_node(dst,
						copy_branch(dst, src->left),
						token_mul,
						deriv(dst, src->right, var)
					)
				);
			case token_add:
				return new_node(dst,
					deriv(dst, src->left, var),
					token_add,
					deriv(dst, src->right, var)
				);
			case token_sub:
				return new_node(dst,
					deriv(dst, src->left, var),
					token_sub,
					deriv(dst, src->right, var)
				);
			case token_neg:
				return new_node(dst, 0, token_neg, deriv(dst, src->right, var));
			case token_err:
				return 0;
			default:
				error::set_num(error::unknown_token);
				return 0;
		}
	}

	int deriv(struct tree *dst, struct tree *src, int var)
	{
		error::set_num_pos(error::empty_expression, 0);
		reset_tree(dst);
		dst->root = deriv(dst, src->root, var);
		return 0 != dst->root;
	}
}

