
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "parser.h"
#include "eval.h"
#include "reduce.h"
#include "error.h"

namespace srt
{
	static struct node *reset_num(struct node *node, float value)
	{
		node->left = 0;
		node->right = 0;
		node->token = token_num;
		node->value = value;
		return node;
	}

	static struct node *apply(struct node *left, struct node *right, float (*func)(float, float))
	{
		left->value = func(left->value, right->value);
		return left;
	}

	static void swap(struct node **left, struct node **right)
	{
		struct node *tmp = *left;
		*left = *right;
		*right = tmp;
	}

	static void rotate(struct node **n0, struct node **n1, struct node **n2)
	{
		struct node *tmp = *n0;
		*n0 = *n1;
		*n1 = *n2;
		*n2 = tmp;
	}

	static int num(struct node *node)
	{
		return node->token == token_num;
	}

	static int zero(struct node *node)
	{
		return num(node) && 0 == node->value;
	}

	static int one(struct node *node)
	{
		return num(node) && 1 == node->value;
	}

	static struct node *commutative(struct node *node, int token, float (*func)(float, float))
	{
		if (num(node->left) && token == node->right->token && num(node->right->left)) {
			apply(node->left, node->right->left, func);
			node->right = node->right->right;
			return node;
		}
		if (token == node->left->token && num(node->left->left) && token == node->right->token && num(node->right->left)) {
			apply(node->left->left, node->right->left, func);
			node->right->left = node->left->right;
			node->left->right = node->right;
			return node->left;
		}
		if (token == node->left->token && num(node->left->left)) {
			rotate(&node->left, &node->left->left, &node->right);
			return node;
		}
		if (token == node->right->token && num(node->right->left)) {
			rotate(&node->right, &node->right->left, &node->left);
			swap(&node->left, &node->right);
			return node;
		}
		return node;
	}

	static struct node *reduce(struct node *node)
	{
		switch (node->token) {
			case token_x:
			case token_y:
			case token_z:
			case token_a:
			case token_num:
				return node;
			case token_pow:
				if (0 == node->value)
					return reset_num(node, 1);
				if (1 == node->value)
					return reduce(node->left);
				if (zero(node->left) || one(node->left))
					return node->left;
				if (num(node->left))
					return apply(node->left, node, func_pow);
				node->left = reduce(node->left);
				if (zero(node->left) || one(node->left))
					return node->left;
				if (num(node->left))
					return apply(node->left, node, func_pow);
				return node;
			case token_mul:
				if (zero(node->left) || zero(node->right))
					return reset_num(node, 0);
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_mul);
				if (one(node->left))
					return reduce(node->right);
				if (one(node->right))
					return reduce(node->left);
				node->left = reduce(node->left);
				if (zero(node->left))
					return node->left;
				node->right = reduce(node->right);
				if (zero(node->right))
					return node->right;
				if (!num(node->left) && num(node->right))
					swap(&node->left, &node->right);
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_mul);
				if (one(node->left))
					return reduce(node->right);
				if (one(node->right))
					return reduce(node->left);
				return commutative(node, token_mul, func_mul);
			case token_div:
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_div);
				if (zero(node->left))
					return node->left;
				if (one(node->right))
					return reduce(node->left);
				node->left = reduce(node->left);
				if (zero(node->left))
					return node->left;
				node->right = reduce(node->right);
				if (one(node->right))
					return node->left;
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_div);
				if (num(node->right)) {
					reset_num(node->right, 1.0f / node->right->value);
					swap(&node->left, &node->right);
					node->token = token_mul;
					return node;
				}
				return node;
			case token_add:
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_add);
				if (zero(node->left))
					return reduce(node->right);
				if (zero(node->right))
					return reduce(node->left);
				node->left = reduce(node->left);
				node->right = reduce(node->right);
				if (zero(node->left))
					return node->right;
				if (zero(node->right))
					return node->left;
				if (!num(node->left) && num(node->right))
					swap(&node->left, &node->right);
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_add);
				return commutative(node, token_add, func_add);
			case token_sub:
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_sub);
				if (zero(node->right))
					return reduce(node->left);
				node->left = reduce(node->left);
				node->right = reduce(node->right);
				if (zero(node->right))
					return node->left;
				if (num(node->left) && num(node->right))
					return apply(node->left, node->right, func_sub);
				if (zero(node->left)) {
					node->left = 0;
					node->token = token_neg;
					return node;
				}
				return node;
			case token_neg:
				if (num(node->right))
					return apply(node->right, node->right, func_neg);
				node->right = reduce(node->right);
				if (num(node->right))
					return apply(node->right, node->right, func_neg);
				return node;
			case token_sqrt:
				if (num(node->right))
					return apply(node->right, node->right, func_sqrt);
				node->right = reduce(node->right);
				if (num(node->right))
					return apply(node->right, node->right, func_sqrt);
				return node;
			case token_sin:
				if (num(node->right))
					return apply(node->right, node->right, func_sin);
				node->right = reduce(node->right);
				if (num(node->right))
					return apply(node->right, node->right, func_sin);
				return node;
			case token_cos:
				if (num(node->right))
					return apply(node->right, node->right, func_cos);
				node->right = reduce(node->right);
				if (num(node->right))
					return apply(node->right, node->right, func_cos);
				return node;
			case token_err:
				return 0;
			default:
				error::set_num(error::unknown_token);
				return 0;
		}
	}

	int reduce(struct tree *tree)
	{
		error::set_num_pos(error::empty_expression, 0);
		tree->root = reduce(tree->root);
		return 0 != tree->root;
	}

	struct node *reduce_branch(struct node *node)
	{
		error::set_num_pos(error::empty_expression, 0);
		return reduce(node);
	}
}

