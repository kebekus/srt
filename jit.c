#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Transforms/IPO.h>
#include <stdlib.h>
#include <stdio.h>
#include "cbind.h"
#include "jit.h"

struct jit
{
	LLVMModuleRef module;
	LLVMBuilderRef builder;
	LLVMPassManagerRef pass;
	LLVMExecutionEngineRef engine;
	LLVMMemoryBufferRef bc;
};

static void reset_jit(struct jit *jit)
{
	char *error = 0;
	if (LLVMRemoveModule(jit->engine, jit->module, &jit->module, &error)) {
		fprintf(stderr, "LLVMRemoveModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);
	LLVMDisposeModule(jit->module);
	error = 0;
	if (jit->bc) {
		if (LLVMParseBitcode(jit->bc, &jit->module, &error)) {
			fprintf(stderr, "LLVMParseBitcode:\n%s\n", error);
			LLVMDisposeMessage(error);
			abort();
		}
	} else {
		jit->module = LLVMModuleCreateWithName("jit");
	}
	LLVMDisposeMessage(error);
	LLVMAddModule(jit->engine, jit->module);
}

void parser_reset_jit(struct parser_jit *parser_jit)
{
	reset_jit((struct jit *)parser_jit->data);
}

struct parser_jit *parser_alloc_jit(char *bc, int len)
{
	LLVMLinkInJIT();
	LLVMInitializeNativeTarget();
	struct jit *jit = malloc(sizeof(struct jit));

	jit->bc = 0;
	if (bc && len)
		jit->bc = LLVMGetMemoryBufferFromArray(bc, len);

	char *error = 0;
	if (jit->bc) {
		if (LLVMParseBitcode(jit->bc, &jit->module, &error)) {
			fprintf(stderr, "LLVMParseBitcode:\n%s\n", error);
			LLVMDisposeMessage(error);
			abort();
		}
	} else {
		jit->module = LLVMModuleCreateWithName("jit");
	}
	LLVMDisposeMessage(error);

	error = 0;
	if (LLVMCreateJITCompilerForModule(&jit->engine, jit->module, 2, &error)) {
		fprintf(stderr, "LLVMCreateJITCompilerForModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);

	jit->builder = LLVMCreateBuilder();
	jit->pass = LLVMCreatePassManager();

	LLVMAddTargetData(LLVMGetExecutionEngineTargetData(jit->engine), jit->pass);
	LLVMAddConstantPropagationPass(jit->pass);
	LLVMAddInstructionCombiningPass(jit->pass);
	LLVMAddPromoteMemoryToRegisterPass(jit->pass);
//	LLVMAddDemoteMemoryToRegisterPass(jit->pass);
	LLVMAddGVNPass(jit->pass);
	LLVMAddCFGSimplificationPass(jit->pass);
	LLVMAddFunctionInliningPass(jit->pass);

	struct parser_jit *parser_jit = malloc(sizeof(struct parser_jit));
	parser_jit->data = jit;
	return parser_jit;
}

static LLVMValueRef emit_pow(LLVMBuilderRef builder, LLVMValueRef base, int exp)
{
	if (0 == exp)
		return LLVMConstReal(LLVMFloatType(), 1);
	else if (1 == exp)
		return base;
	else if (exp & 1)
		return LLVMBuildFMul(builder, base, emit_pow(builder, LLVMBuildFMul(builder, base, base, ""), (exp - 1) / 2), "");
	else
		return emit_pow(builder, LLVMBuildFMul(builder, base, base, ""), exp / 2);
}

static LLVMValueRef emit(LLVMBuilderRef builder, struct parser_node *node, LLVMValueRef x, LLVMValueRef y, LLVMValueRef z, LLVMValueRef a)
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
			return LLVMConstReal(LLVMFloatType(), node->value);
		case token_pow:
			return emit_pow(builder, emit(builder, node->left, x, y, z, a), node->value);
		case token_mul:
			return LLVMBuildFMul(builder, emit(builder, node->left, x, y, z, a), emit(builder, node->right, x, y, z, a), "");
		case token_div:
			return LLVMBuildFDiv(builder, emit(builder, node->left, x, y, z, a), emit(builder, node->right, x, y, z, a), "");
		case token_add:
			return LLVMBuildFAdd(builder, emit(builder, node->left, x, y, z, a), emit(builder, node->right, x, y, z, a), "");
		case token_sub:
			return LLVMBuildFSub(builder, emit(builder, node->left, x, y, z, a), emit(builder, node->right, x, y, z, a), "");
		case token_neg:
			return LLVMBuildFNeg(builder,  emit(builder, node->right, x, y, z, a), "");
		default:
			return 0;
	}
}

void parser_jit_build(struct parser_jit *parser_jit, struct parser_tree *tree, char *name)
{
	struct jit *jit = parser_jit->data;
	LLVMValueRef func;
	if (LLVMFindFunction(jit->engine, name, &func)) {
		LLVMTypeRef args[] = { LLVMFloatType(), LLVMFloatType(), LLVMFloatType(), LLVMFloatType() };
		func = LLVMAddFunction(jit->module, name, LLVMFunctionType(LLVMFloatType(), args, 4, 0));
		LLVMSetFunctionCallConv(func, LLVMCCallConv);
	}
	LLVMValueRef x = LLVMGetParam(func, 0);
	LLVMSetValueName(x, "x");
	LLVMValueRef y = LLVMGetParam(func, 1);
	LLVMSetValueName(y, "y");
	LLVMValueRef z = LLVMGetParam(func, 2);
	LLVMSetValueName(z, "z");
	LLVMValueRef a = LLVMGetParam(func, 3);
	LLVMSetValueName(a, "a");
	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(func, "entry");
	LLVMPositionBuilderAtEnd(jit->builder, entry);
	LLVMBuildRet(jit->builder, emit(jit->builder, tree->root, x, y, z, a));
}

void parser_jit_opt(struct parser_jit *parser_jit)
{
	struct jit *jit = parser_jit->data;
	char *error = 0;
	if (LLVMVerifyModule(jit->module, LLVMReturnStatusAction, &error)) {
		fprintf(stderr, "LLVMVerifyModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);

	LLVMRunPassManager(jit->pass, jit->module);
//	LLVMDumpModule(jit->module);
}

void *parser_jit_func(struct parser_jit *parser_jit, char *name)
{
	struct jit *jit = parser_jit->data;
	LLVMValueRef func;
	if (LLVMFindFunction(jit->engine, name, &func)) {
		fprintf(stderr, "LLVMFindFunction\n");
		abort();
	}
	return LLVMGetPointerToFunction(jit->engine, func);
}

void parser_free_jit(struct parser_jit *parser_jit)
{
	struct jit *jit = parser_jit->data;
	LLVMDisposeMemoryBuffer(jit->bc);
	LLVMDisposeExecutionEngine(jit->engine);
	LLVMDisposePassManager(jit->pass);
	LLVMDisposeBuilder(jit->builder);
//	LLVMDisposeModule(jit->module); engine owns module ..
	free(jit);
	free(parser_jit);
}

