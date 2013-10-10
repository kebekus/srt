
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "parser.h"
#include <math.h>

namespace srt
{
	float func_pow(float left, float right)
	{
		return powf(left, right);
	}

	float func_mul(float left, float right)
	{
		return left * right;
	}

	float func_div(float left, float right)
	{
		return left / right;
	}

	float func_add(float left, float right)
	{
		return left + right;
	}

	float func_sub(float left, float right)
	{
		return left - right;
	}

	float func_neg(float left, float right)
	{
		(void)left;
		return -right;
	}

	float func_sqrt(float left, float right)
	{
		(void)left;
		return sqrtf(right);
	}

	float func_sin(float left, float right)
	{
		(void)left;
		return sinf(right);
	}

	float func_cos(float left, float right)
	{
		(void)left;
		return cosf(right);
	}

	float eval_branch(struct node *node, float x, float y, float z, float a)
	{
		switch (node->token) {
			case token_x:
				return x;
			case token_y:
				return y;
			case token_z:
				return z;
			case token_a:
				return a;
			case token_num:
				return node->value;
			case token_pow:
				return func_pow(eval_branch(node->left, x, y, z, a), node->value);
			case token_mul:
				return func_mul(eval_branch(node->left, x, y, z, a), eval_branch(node->right, x, y, z, a));
			case token_div:
				return func_div(eval_branch(node->left, x, y, z, a), eval_branch(node->right, x, y, z, a));
			case token_add:
				return func_add(eval_branch(node->left, x, y, z, a), eval_branch(node->right, x, y, z, a));
			case token_sub:
				return func_sub(eval_branch(node->left, x, y, z, a), eval_branch(node->right, x, y, z, a));
			case token_neg:
				return func_neg(0, eval_branch(node->right, x, y, z, a));
			case token_sqrt:
				return func_sqrt(0, eval_branch(node->right, x, y, z, a));
			case token_sin:
				return func_sin(0, eval_branch(node->right, x, y, z, a));
			case token_cos:
				return func_cos(0, eval_branch(node->right, x, y, z, a));
			default:
				return 0;
		}
	}

	float eval(struct tree *tree, float x, float y, float z, float a)
	{
		return eval_branch(tree->root, x, y, z, a);
	}
}
