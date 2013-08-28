
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

struct parser::jit::data {
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

void parser::jit::reset()
{
	char *error = 0;
	if (LLVMRemoveModule(data->engine, data->module, &data->module, &error)) {
		fprintf(stderr, "LLVMRemoveModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);
	LLVMDisposeModule(data->module);
	error = 0;
	if (data->bc) {
		if (LLVMParseBitcode(data->bc, &data->module, &error)) {
			fprintf(stderr, "LLVMParseBitcode:\n%s\n", error);
			LLVMDisposeMessage(error);
			abort();
		}
	} else {
		data->module = LLVMModuleCreateWithName("jit");
	}
	LLVMDisposeMessage(error);
	LLVMAddModule(data->engine, data->module);
}

parser::jit::jit(char *code, int len)
{
	LLVMLinkInJIT();
	LLVMInitializeNativeTarget();
	data = new struct data;

	data->bc = 0;
	if (code && len)
		data->bc = LLVMGetMemoryBufferFromArray(code, len);

	char *error = 0;
	if (data->bc) {
		if (LLVMParseBitcode(data->bc, &data->module, &error)) {
			fprintf(stderr, "LLVMParseBitcode:\n%s\n", error);
			LLVMDisposeMessage(error);
			abort();
		}
	} else {
		data->module = LLVMModuleCreateWithName("jit");
	}
	LLVMDisposeMessage(error);

	error = 0;
	if (LLVMCreateJITCompilerForModule(&data->engine, data->module, 2, &error)) {
		fprintf(stderr, "LLVMCreateJITCompilerForModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);

	data->builder = LLVMCreateBuilder();
	data->pass = LLVMCreatePassManager();

	LLVMAddTargetData(LLVMGetExecutionEngineTargetData(data->engine), data->pass);
	LLVMAddConstantPropagationPass(data->pass);
	LLVMAddInstructionCombiningPass(data->pass);
	LLVMAddPromoteMemoryToRegisterPass(data->pass);
//	LLVMAddDemoteMemoryToRegisterPass(data->pass);
	LLVMAddGVNPass(data->pass);
	LLVMAddCFGSimplificationPass(data->pass);
	LLVMAddFunctionInliningPass(data->pass);

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

void parser::jit::build(struct parser_tree *tree, const char *name)
{
	LLVMValueRef func;
	if (LLVMFindFunction(data->engine, name, &func)) {
		LLVMTypeRef args[] = {
			LLVMVectorType(LLVMFloatType(), 4),
			LLVMVectorType(LLVMFloatType(), 4),
			LLVMVectorType(LLVMFloatType(), 4),
			LLVMFloatType()
		};
		func = LLVMAddFunction(data->module, name, LLVMFunctionType(LLVMVectorType(LLVMFloatType(), 4), args, 4, 0));
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
	LLVMPositionBuilderAtEnd(data->builder, entry);
	LLVMBuildRet(data->builder, emit(data->builder, tree->root, x, y, z, splat(data->builder, a)));
}

void parser::jit::link()
{
	char *error = 0;
	if (LLVMVerifyModule(data->module, LLVMReturnStatusAction, &error)) {
		fprintf(stderr, "LLVMVerifyModule:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);

	LLVMRunPassManager(data->pass, data->module);
//	LLVMDumpModule(data->module);
#if 0
	error = 0;
	if (LLVMPrintModuleToFile(data->module, "module.ll", &error)) {
		fprintf(stderr, "LLVMPrintModuleToFile:\n%s\n", error);
		LLVMDisposeMessage(error);
		abort();
	}
	LLVMDisposeMessage(error);
#endif
}

void *parser::jit::func(const char *name)
{
	LLVMValueRef func;
	if (LLVMFindFunction(data->engine, name, &func)) {
		fprintf(stderr, "LLVMFindFunction\n");
		abort();
	}
	return LLVMGetPointerToFunction(data->engine, func);
}

parser::jit::~jit()
{
	LLVMDisposeMemoryBuffer(data->bc);
	LLVMDisposeExecutionEngine(data->engine);
	LLVMDisposePassManager(data->pass);
	LLVMDisposeBuilder(data->builder);
//	LLVMDisposeModule(data->module); engine owns module ..
	delete data;
}

