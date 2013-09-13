
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "error.h"
#include "reduce.h"

struct parser_node *node_set_err_str(const char *what)
{
	set_err_str(what);
	return 0;
}

struct parser_tree *parser_alloc_tree(int size)
{
	struct parser_tree *tree = (struct parser_tree *)malloc(sizeof(struct parser_tree));
	tree->begin = (struct parser_node *)malloc(sizeof(struct parser_node)*size);
	tree->cur = tree->begin;
	tree->end = tree->begin + size;
	tree->root = 0;
	return tree;
}

void parser_reset_tree(struct parser_tree *tree)
{
	tree->root = 0;
	tree->cur = tree->begin;
}

void parser_free_tree(struct parser_tree *tree)
{
	free(tree->begin);
	free(tree);
}

struct parser_node *parser_new_node(struct parser_tree *tree, struct parser_node *left, int token, struct parser_node *right)
{
	if (!token)
		return 0;
	if (tree->cur >= tree->end)
		return node_set_err_str("not enough nodes");
	tree->cur->left = left;
	tree->cur->right = right;
	tree->cur->token = token;
	tree->cur->value = 0;
	return (tree->cur)++;
}

struct parser_node *parser_new_num(struct parser_tree *tree, float value)
{
	if (tree->cur >= tree->end)
		return node_set_err_str("not enough nodes");
	tree->cur->left = 0;
	tree->cur->right = 0;
	tree->cur->token = token_num;
	tree->cur->value = value;
	return (tree->cur)++;
}

struct parser_node *parser_new_pow(struct parser_tree *tree, struct parser_node *left, float exponent)
{
	if (tree->cur >= tree->end)
		return node_set_err_str("not enough nodes");
	tree->cur->left = left;
	tree->cur->right = 0;
	tree->cur->token = token_pow;
	tree->cur->value = exponent;
	return (tree->cur)++;
}

struct parser_node *parser_new_var(struct parser_tree *tree, int token)
{
	if (!token)
		return 0;
	if (tree->cur >= tree->end)
		return node_set_err_str("not enough nodes");
	tree->cur->left = 0;
	tree->cur->right = 0;
	tree->cur->token = token;
	tree->cur->value = 0;
	return (tree->cur)++;
}

int parser_token_prio(int token)
{
	switch (token) {
		case token_paran:
			return -1;
		case token_sqrt:
		case token_sin:
		case token_cos:
			return 4;
		case token_pow:
			return 3;
		case token_div:
			return 2;
		case token_mul:
			return 1;
		case token_add:
		case token_sub:
		case token_neg:
		default:
			return 0;
	}
}

static int integer(float f)
{
	int i = f;
	return f == (float)i;
}

static struct parser_node *handle_pow(struct parser_node *left, struct parser_node *right)
{
	if (token_num != right->token)
		right = parser_reduce_branch(right);
	if (token_num != right->token)
		return node_set_err_str("exponent does not reduce to number");
	if (token_num != left->token && !integer(right->value))
		return node_set_err_str("exponent not integer");
	if (right->value < 0)
		return node_set_err_str("exponent is negative");
	right->left = left;
	right->right = 0;
	right->token = token_pow;
	return right;
}

static struct parser_node *handle_div(struct parser_tree *tree, struct parser_node *left, struct parser_node *right)
{
	if (token_num != right->token)
		right = parser_reduce_branch(right);
	if (token_num != right->token)
		return node_set_err_str("divisor does not reduce to number");
	if (0 == right->value)
		return node_set_err_str("divisor is zero");
	return parser_new_node(tree, left, token_div, right);
}

static struct parser_node *handle_sqrt(struct parser_tree *tree, struct parser_node *right)
{
	if (token_num != right->token)
		right = parser_reduce_branch(right);
	if (token_num != right->token)
		return node_set_err_str("argument does not reduce to number");
	if (0 > right->value)
		return node_set_err_str("argument is negative");
	return parser_new_node(tree, 0, token_sqrt, right);
}

static struct parser_node *handle_sin(struct parser_tree *tree, struct parser_node *right)
{
	if (token_num != right->token)
		right = parser_reduce_branch(right);
	if (token_num != right->token)
		return node_set_err_str("argument does not reduce to number");
	return parser_new_node(tree, 0, token_sin, right);
}

static struct parser_node *handle_cos(struct parser_tree *tree, struct parser_node *right)
{
	if (token_num != right->token)
		right = parser_reduce_branch(right);
	if (token_num != right->token)
		return node_set_err_str("argument does not reduce to number");
	return parser_new_node(tree, 0, token_cos, right);
}

static struct parser_node *handle_node(struct parser_tree *tree, struct parser_node *left, int token, struct parser_node *right)
{
	if (!token)
		return 0;
	if (token_pow == token)
		return handle_pow(left, right);
	if (token_div == token)
		return handle_div(tree, left, right);
	if (token_sqrt == token)
		return handle_sqrt(tree, right);
	if (token_sin == token)
		return handle_sin(tree, right);
	if (token_cos == token)
		return handle_cos(tree, right);
	return parser_new_node(tree, left, token, right);
}

static float a2f(const char *s)
{
	float r = 0;
	for (; '0' <= *s && *s <= '9'; s++)
		r = 10 * r + (*s - '0');
	if ('.' != *s++)
		return r;
	for (float d = 10; '0' <= *s && *s <= '9'; s++, d *= 10)
		r += (*s - '0') / d;
	return r;
}

static struct parser_node *handle_num(struct parser_tree *tree, int *pos, const char *str, int len)
{
	char tmp[20];
	int i = 0;
	int dot = 0;
	while (*pos < len && ('.' == str[*pos] || ('0' <= str[*pos] && str[*pos] <= '9'))) {
		if ('.' == str[*pos] && ++dot > 1)
			return node_set_err_str("there can be only one");
		tmp[i] = str[*pos];
		if (++i >= 20)
			return node_set_err_str("number to long");
		(*pos)++;
	}
	(*pos)--;
	tmp[i] = 0;
	return parser_new_num(tree, a2f(tmp));
}

static int cmp_word(const char *word, int *pos, const char *str, int len)
{
	int word_len = strlen(word);
	if ((*pos + word_len) > len || strncmp(word, str + *pos, word_len))
		return set_err_str("unknown");
	*pos += word_len - 1;
	return 1;
}

#define ELEMENTS (128)

struct element {
	struct parser_node *node;
	int token;
};

static struct element element(struct parser_node *node, int token)
{
	return (struct element){ node, token };
}

static struct element stack[ELEMENTS];
static int elements = 0;

static int empty()
{
	return !elements;
}

static struct element top()
{
	return stack[elements-1];
}

#if 1
static void print_stack(){}
#else
static void print_stack()
{
	for (int i = 0; i < elements; i++)
		fprintf(stderr, "[%02d] ", stack[i].token);
	for (int i = elements; i < ELEMENTS; i++)
		fprintf(stderr, "[  ] ");
	fprintf(stderr, "\n");
}
#endif

static struct element pop()
{
	struct element res = stack[--elements];
	print_stack();
	return res;
}

static int push(struct element e)
{
	if (elements >= ELEMENTS)
		return set_err_str("stack overflow");
	stack[elements] = e;
	int res = ++elements;
	print_stack();
	return res;
}

static int operand(struct parser_node **left, int op, struct parser_node **right, struct parser_node *node)
{
	if (!node)
		return 0;
	if (!*left && !op) {
		*left = node;
	} else if (op && !*right) {
		*right = node;
	} else {
		return set_err_str("syntax error");
	}
	return 1;
}

static int binary_operator(struct parser_tree *tree, struct parser_node **left, int *op, struct parser_node **right, int token)
{
	if (!token)
		return 0;
	if (*left && !*op) {
		*op = token;
	} else if (*op && *right) {
		if (parser_token_prio(*op) >= parser_token_prio(token)) {
			if (!(*left = handle_node(tree, *left, *op, *right)))
				return 0;
			*op = token;
			*right = 0;
		} else {
			if (!push(element(*left, *op)))
				return 0;
			*left = *right;
			*op = token;
			*right = 0;
			return 1;
		}
	} else {
		return set_err_str("syntax error");
	}
	while (!empty() && parser_token_prio(top().token) >= parser_token_prio(*op)) {
		struct element e = pop();
		if (!(*left = handle_node(tree, e.node, e.token, *left)))
			return 0;
	}
	return 1;
}

static int unary_operator(struct parser_node **left, int *op, struct parser_node **right, int token)
{
	if (!token)
		return 0;
	if (!*left && !*op) {
		*op = token;
		return 1;
	} else if (*op && !*right) {
		if (!push(element(*left, *op)))
			return 0;
		*left = 0;
		*op = token;
		*right = 0;
		return 1;
	} else {
		return set_err_str("syntax error");
	}
}

static int opening_paran(struct parser_node **left, int *op, struct parser_node **right)
{
	if (!*left && !*op) {
		if (!push(element(0, token_paran)))
			return 0;
	} else if (*op && !*right) {
		if (!push(element(*left, *op)))
			return 0;
		if (!push(element(0, token_paran)))
			return 0;
		*left = 0;
		*op = 0;
		*right = 0;
	} else {
		return set_err_str("syntax error");
	}
	return 1;
}

static int closing_paran(struct parser_tree *tree, struct parser_node **left, int *op, struct parser_node **right)
{
	if (*op && *right) {
		if (!(*left = handle_node(tree, *left, *op, *right)))
			return 0;
		*op = 0;
		*right = 0;
	} else if (!*left) {
		return set_err_str("syntax error");
	}
	while (!empty() && top().token != token_paran) {
		struct element e = pop();
		if (!(*left = handle_node(tree, e.node, e.token, *left)))
			return 0;
	}
	if (empty())
		return set_err_str("opening parenthese missing");
	pop();
	if (!empty() && top().token != token_paran) {
		struct element e = pop();
		*right = *left;
		*op = e.token;
		*left = e.node;
	}
	return 1;
}

int parser_parse(struct parser_tree *tree, const char *str)
{
	set_err_str_pos("empty expression", 0);
	parser_reset_tree(tree);
	elements = 0;
	struct parser_node *left = 0;
	struct parser_node *right = 0;
	int op = 0;
	int len = strlen(str);
	for (int pos = 0; pos < len; pos++) {
		switch (str[pos]) {
			case ' ':
				break;
			case '.':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (!operand(&left, op, &right, handle_num(tree, &pos, str, len)))
					return set_err_pos(pos);
				break;
			case 'x':
				if (!operand(&left, op, &right, parser_new_var(tree, token_x)))
					return set_err_pos(pos);
				break;
			case 'y':
				if (!operand(&left, op, &right, parser_new_var(tree, token_y)))
					return set_err_pos(pos);
				break;
			case 'z':
				if (!operand(&left, op, &right, parser_new_var(tree, token_z)))
					return set_err_pos(pos);
				break;
			case 'a':
				if (!operand(&left, op, &right, parser_new_var(tree, token_a)))
					return set_err_pos(pos);
				break;
			case 'c':
				if (!cmp_word("cos", &pos, str, len) || !unary_operator(&left, &op, &right, token_cos))
					return set_err_pos(pos);
				break;
			case 's':
				if (cmp_word("sqrt", &pos, str, len)) {
					if (!unary_operator(&left, &op, &right, token_sqrt))
						return set_err_pos(pos);
				} else if (cmp_word("sin", &pos, str, len)) {
					if (!unary_operator(&left, &op, &right, token_sin))
						return set_err_pos(pos);
				} else {
					return set_err_pos(pos);
				}
				break;
			case 'P':
				if (!cmp_word("PI", &pos, str, len) || !operand(&left, op, &right, parser_new_num(tree, 3.14159265358979323846)))
					return set_err_pos(pos);
				break;
			case '^':
				if (!binary_operator(tree, &left, &op, &right, token_pow))
					return set_err_pos(pos);
				break;
			case '*':
				if (!binary_operator(tree, &left, &op, &right, token_mul))
					return set_err_pos(pos);
				break;
			case '/':
				if (!binary_operator(tree, &left, &op, &right, token_div))
					return set_err_pos(pos);
				break;
			case '+':
				if (!binary_operator(tree, &left, &op, &right, token_add))
					return set_err_pos(pos);
				break;
			case '-':
				if (left || op) {
					if (!binary_operator(tree, &left, &op, &right, token_sub))
						return set_err_pos(pos);
				} else {
					if (!unary_operator(&left, &op, &right, token_neg))
						return set_err_pos(pos);
				}
				break;
			case '(':
				if (!opening_paran(&left, &op, &right))
					return set_err_pos(pos);
				break;
			case ')':
				if (!closing_paran(tree, &left, &op, &right))
					return set_err_pos(pos);
				break;
			default:
				return set_err_str_pos("unknown char", pos);
		}
	}
	if (op && right) {
		if (!(left = handle_node(tree, left, op, right)))
			return set_err_pos(len-1);
	} else if (op || right) {
		return set_err_str_pos("incomplete expression", len-1);
	}
	while (!empty() && top().token != token_paran) {
		struct element e = pop();
		if (!(left = handle_node(tree, e.node, e.token, left)))
			return set_err_pos(len-1);
	}
	if (!empty())
		return set_err_str_pos("closing parenthese missing", len-1);
	tree->root = left;
	return 0 != left;
}

