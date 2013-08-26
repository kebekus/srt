
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Transforms/IPO.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <stdlib.h>
#include <stdio.h>
#include "jit.h"

struct jit
{
	LLVMModuleRef module;
	LLVMBuilderRef builder;
	LLVMPassManagerRef pass;
	LLVMExecutionEngineRef engine;
	LLVMMemoryBufferRef bc;
};

LLVMMemoryBufferRef LLVMGetMemoryBufferFromArray(const char *Array, unsigned Length)
{
	assert(Array);
	assert(Length);
	return llvm::wrap(llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(Array, Length), "", false));
}

void *LLVMGetPointerToFunction(LLVMExecutionEngineRef EE, LLVMValueRef F)
{
	return llvm::unwrap(EE)->getPointerToFunction(llvm::unwrap<llvm::Function>(F));
}

void parser_reset_jit(struct parser_jit *parser_jit)
{
	struct jit *jit = (struct jit *)parser_jit->data;
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

struct parser_jit *parser_alloc_jit(char *bc, int len)
{
	LLVMLinkInJIT();
	LLVMInitializeNativeTarget();
	struct jit *jit = (struct jit *)malloc(sizeof(struct jit));

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

	struct parser_jit *parser_jit = (struct parser_jit *)malloc(sizeof(struct parser_jit));
	parser_jit->data = jit;
	return parser_jit;
}

static LLVMValueRef splat(LLVMBuilderRef builder, LLVMValueRef elem)
{
	LLVMValueRef zero = LLVMConstInt(LLVMInt32Type(), 0, 0);
	LLVMValueRef zeros[4] = { zero, zero, zero, zero };
	LLVMValueRef mask = LLVMConstVector(zeros, 4);
	LLVMValueRef vector = LLVMGetUndef(LLVMVectorType(LLVMFloatType(), 4));
	vector = LLVMBuildInsertElement(builder, vector, elem, zero, "");
	return LLVMBuildShuffleVector(builder, vector, vector, mask, "");
}

static LLVMValueRef emit_pow(LLVMBuilderRef builder, LLVMValueRef base, int exp)
{
	if (0 == exp)
		return splat(builder, LLVMConstReal(LLVMFloatType(), 1));
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
			return splat(builder, LLVMConstReal(LLVMFloatType(), node->value));
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

void parser_jit_build(struct parser_jit *parser_jit, struct parser_tree *tree, const char *name)
{
	struct jit *jit = (struct jit *)parser_jit->data;
	LLVMValueRef func;
	if (LLVMFindFunction(jit->engine, name, &func)) {
		LLVMTypeRef args[] = {
			LLVMVectorType(LLVMFloatType(), 4),
			LLVMVectorType(LLVMFloatType(), 4),
			LLVMVectorType(LLVMFloatType(), 4),
			LLVMFloatType()
		};
		func = LLVMAddFunction(jit->module, name, LLVMFunctionType(LLVMVectorType(LLVMFloatType(), 4), args, 4, 0));
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
	LLVMBuildRet(jit->builder, emit(jit->builder, tree->root, x, y, z, splat(jit->builder, a)));
}

void parser_jit_link(struct parser_jit *parser_jit)
{
	struct jit *jit = (struct jit *)parser_jit->data;
	char *error = 0;
	if (LLVMVerifyModule(jit->module, LLVMReturnStatusAction, &error)) {
		fprintf(stderr, "LLVMVerifyModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);

	LLVMRunPassManager(jit->pass, jit->module);
//	LLVMDumpModule(jit->module);
#if 0
	error = 0;
	if (LLVMPrintModuleToFile(jit->module, "module.ll", &error)) {
		fprintf(stderr, "LLVMPrintModuleToFile:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);
#endif
}

void *parser_jit_func(struct parser_jit *parser_jit, const char *name)
{
	struct jit *jit = (struct jit *)parser_jit->data;
	LLVMValueRef func;
	if (LLVMFindFunction(jit->engine, name, &func)) {
		fprintf(stderr, "LLVMFindFunction\n");
		abort();
	}
	return LLVMGetPointerToFunction(jit->engine, func);
}

void parser_free_jit(struct parser_jit *parser_jit)
{
	struct jit *jit = (struct jit *)parser_jit->data;
	LLVMDisposeMemoryBuffer(jit->bc);
	LLVMDisposeExecutionEngine(jit->engine);
	LLVMDisposePassManager(jit->pass);
	LLVMDisposeBuilder(jit->builder);
//	LLVMDisposeModule(jit->module); engine owns module ..
	free(jit);
	free(parser_jit);
}

