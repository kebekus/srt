
#include "parser.h"
#include <math.h>

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

float parser_eval_branch(struct parser_node *node, float x, float y, float z, float a)
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
			return func_pow(parser_eval_branch(node->left, x, y, z, a), node->value);
		case token_mul:
			return func_mul(parser_eval_branch(node->left, x, y, z, a), parser_eval_branch(node->right, x, y, z, a));
		case token_div:
			return func_div(parser_eval_branch(node->left, x, y, z, a), parser_eval_branch(node->right, x, y, z, a));
		case token_add:
			return func_add(parser_eval_branch(node->left, x, y, z, a), parser_eval_branch(node->right, x, y, z, a));
		case token_sub:
			return func_sub(parser_eval_branch(node->left, x, y, z, a), parser_eval_branch(node->right, x, y, z, a));
		case token_neg:
			return func_neg(0, parser_eval_branch(node->right, x, y, z, a));
		case token_sqrt:
			return func_sqrt(0, parser_eval_branch(node->right, x, y, z, a));
		default:
			return 0;
	}
}

float parser_eval(struct parser_tree *tree, float x, float y, float z, float a)
{
	return parser_eval_branch(tree->root, x, y, z, a);
}

