
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include "jit.h"

struct jit {
	char *object;
	char *code;
	char *module;
	void *handle;
	FILE *file;
};

static int counter = 0;

static int emit_num(FILE *file, float value)
{
	fprintf(file, "\tv4sf _%d = v4sf_set1(%g);\n", counter, value);
	return counter++;
}

static int emit_var(FILE *file, char *name)
{
	fprintf(file, "\tv4sf _%d = v4sf_set1(%s);\n", counter, name);
	return counter++;
}

static int emit_vec(FILE *file, char *name)
{
	fprintf(file, "\tv4sf _%d = %s;\n", counter, name);
	return counter++;
}

static int emit_mul(FILE *file, int left, int right)
{
	fprintf(file, "\tv4sf _%d = _%d * _%d;\n", counter, left, right);
	return counter++;
}

static int emit_div(FILE *file, int left, int right)
{
	fprintf(file, "\tv4sf _%d = _%d / _%d;\n", counter, left, right);
	return counter++;
}

static int emit_add(FILE *file, int left, int right)
{
	fprintf(file, "\tv4sf _%d = _%d + _%d;\n", counter, left, right);
	return counter++;
}

static int emit_sub(FILE *file, int left, int right)
{
	fprintf(file, "\tv4sf _%d = _%d - _%d;\n", counter, left, right);
	return counter++;
}

static int emit_neg(FILE *file, int right)
{
	fprintf(file, "\tv4sf _%d = - _%d;\n", counter, right);
	return counter++;
}

static int emit_pow(FILE *file, int base, int exp)
{
	if (0 == exp)
		return emit_num(file, 1);
	else if (1 == exp)
		return base;
	else if (exp & 1)
		return emit_mul(file, base, emit_pow(file, emit_mul(file, base, base), (exp - 1) / 2));
	else
		return emit_pow(file, emit_mul(file, base, base), exp / 2);
}

static int emit(FILE *file, struct parser_node *node)
{
	switch (node->token) {
		case token_x:
			return emit_vec(file, "x");
		case token_y:
			return emit_vec(file, "y");
		case token_z:
			return emit_vec(file, "z");
		case token_a:
			return emit_var(file, "a");
		case token_num:
			return emit_num(file, node->value);
		case token_pow:
			return emit_pow(file, emit(file, node->left), node->value);
		case token_mul:
			return emit_mul(file, emit(file, node->left), emit(file, node->right));
		case token_div:
			return emit_div(file, emit(file, node->left), emit(file, node->right));
		case token_add:
			return emit_add(file, emit(file, node->left), emit(file, node->right));
		case token_sub:
			return emit_sub(file, emit(file, node->left), emit(file, node->right));
		case token_neg:
			return emit_neg(file, emit(file, node->right));
		default:
			return -1;
	}
}

void parser_reset_jit(struct parser_jit *parser_jit)
{
	struct jit *jit = parser_jit->data;
	if (jit->file)
		fclose(jit->file);
	jit->file = fopen(jit->code, "w");
	if (!jit->file)
		exit(1);
	fprintf(jit->file, "#include \"vector.h\"\n");
	if (jit->handle)
		dlclose(jit->handle);
	jit->handle = 0;
}

struct parser_jit *parser_alloc_jit(char *object, char *code, char *module)
{
	struct jit *jit = malloc(sizeof(struct parser_jit));
	jit->object = object;
	jit->code = code;
	jit->module = module;
	jit->handle = 0;
	jit->file = fopen(jit->code, "w");
	if (!jit->file)
		exit(1);
	fprintf(jit->file, "#include \"vector.h\"\n");
	struct parser_jit *parser_jit = malloc(sizeof(struct parser_jit));
	parser_jit->data = jit;
	return parser_jit;
}

void parser_jit_build(struct parser_jit *parser_jit, struct parser_tree *tree, char *name)
{
	struct jit *jit = parser_jit->data;
	if (!jit->file)
		exit(1);
	counter = 0;
	fprintf(jit->file, "v4sf %s(v4sf x, v4sf y, v4sf z, float a)\n{\n", name);
	fprintf(jit->file, "\t(void)x; (void)y; (void)z; (void)a;\n");
	fprintf(jit->file, "\treturn _%d;\n}\n", emit(jit->file, tree->root));
}

void parser_jit_link(struct parser_jit *parser_jit)
{
	struct jit *jit = parser_jit->data;
	if (jit->handle)
		exit(1);
	if (!jit->file || fclose(jit->file))
		exit(1);
	jit->file = 0;
	char str[256];
	snprintf(str, 256, "%s -o %s %s %s -shared -fPIC %s", CC, jit->module, jit->code, jit->object, CFLAGS);
//	fprintf(stderr, "executing: \"%s\"\n", str);
	if (system(str))
		exit(1);

	jit->handle = dlopen(jit->module, RTLD_LAZY|RTLD_DEEPBIND);
	if (!jit->handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}
}

void *parser_jit_func(struct parser_jit *parser_jit, char *name)
{
	struct jit *jit = parser_jit->data;
	void *ptr = dlsym(jit->handle, name);
	if (!ptr) {
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}
	return ptr;
}

void parser_free_jit(struct parser_jit *parser_jit)
{
	struct jit *jit = parser_jit->data;
	if (jit->handle)
		dlclose(jit->handle);
	if (jit->file)
		fclose(jit->file);
	free(jit);
	free(parser_jit);
}

