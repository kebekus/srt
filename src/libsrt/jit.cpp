
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <llvm/Analysis/Verifier.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/ExecutionEngine/MCJIT.h>

#include <iostream>
#include "jit.h"

struct parser::jit::impl {
	llvm::Module *module;
	llvm::ExecutionEngine *engine;
	llvm::MemoryBuffer *bitcode;
	llvm::Value *x, *y, *z, *a;
	llvm::Type *scalar_type;
	llvm::VectorType *vector_type;
	llvm::FunctionType *function_type;
	llvm::LLVMContext context;
	llvm::IRBuilder<> builder;
	llvm::PassManager pass_man;
	impl(char *code, int len);
	~impl();
	void reset();
	llvm::Value *emit_pow(llvm::Value *base, int exp);
	llvm::Value *emit(struct parser::node *node);
	void build(struct parser::tree *tree, const char *name);
	void link();
	void *func(const char *name);
};

inline void parser::jit::impl::reset()
{
	delete engine;
	if (bitcode) {
		std::string error;
		module = llvm::ParseBitcodeFile(bitcode, context, &error);
		if (!module) {
			std::cerr << error << std::endl;
			abort();
		}
	} else {
		module = new llvm::Module("jit", context);
	}

	llvm::EngineBuilder engine_builder(module);
	engine_builder.setEngineKind(llvm::EngineKind::JIT);
	std::string error;
	engine_builder.setErrorStr(&error);
	engine_builder.setUseMCJIT(true);
	engine_builder.setOptLevel(llvm::CodeGenOpt::Aggressive);
	engine = engine_builder.create();
	if (!engine) {
		std::cerr << error << std::endl;
		abort();
	}
}

inline parser::jit::impl::impl(char *code, int len) :
	module(0),
	engine(0),
	bitcode(0),
	context(),
	builder(context),
	pass_man()
{
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	if (code && len)
		bitcode = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(code, len), "", false);

	reset();

	// TODO: get type from bitcode
	scalar_type = llvm::Type::getFloatTy(context);
	vector_type = llvm::VectorType::get(scalar_type, 4);
	llvm::Type *args[4] = { vector_type, vector_type, vector_type, scalar_type };
	function_type = llvm::FunctionType::get(vector_type, args, 0);

	pass_man.add(new llvm::DataLayout(*engine->getDataLayout()));
	pass_man.add(llvm::createConstantPropagationPass());
	pass_man.add(llvm::createInstructionCombiningPass());
	pass_man.add(llvm::createPromoteMemoryToRegisterPass());
	pass_man.add(llvm::createGVNPass());
	pass_man.add(llvm::createCFGSimplificationPass());
	pass_man.add(llvm::createFunctionInliningPass());
}

inline llvm::Value *parser::jit::impl::emit_pow(llvm::Value *base, int exp)
{
	if (0 == exp)
		return llvm::ConstantFP::get(vector_type, 1);
	else if (1 == exp)
		return base;
	else if (exp & 1)
		return builder.CreateFMul(base, emit_pow(builder.CreateFMul(base, base, ""), (exp - 1) / 2), "");
	else
		return emit_pow(builder.CreateFMul(base, base, ""), exp / 2);
}

inline llvm::Value *parser::jit::impl::emit(struct parser::node *node)
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
			return llvm::ConstantFP::get(vector_type, node->value);
		case token_pow:
			return emit_pow(emit(node->left), node->value);
		case token_mul:
			return builder.CreateFMul(emit(node->left), emit(node->right), "");
		case token_div:
			return builder.CreateFDiv(emit(node->left), emit(node->right), "");
		case token_add:
			return builder.CreateFAdd(emit(node->left), emit(node->right), "");
		case token_sub:
			return builder.CreateFSub(emit(node->left), emit(node->right), "");
		case token_neg:
			return builder.CreateFNeg(emit(node->right), "");
		default:
			return 0;
	}
}

inline void parser::jit::impl::build(struct parser::tree *tree, const char *name)
{
	llvm::Function *func = engine->FindFunctionNamed(name);
	if (!func) {
		func = llvm::Function::Create(function_type, llvm::GlobalValue::ExternalLinkage, name, module);
		func->setCallingConv(llvm::CallingConv::C);
	}
	llvm::Function::arg_iterator AI = func->arg_begin();
	x = AI++;
	x->setName("x");
	y = AI++;
	y->setName("y");
	z = AI++;
	z->setName("z");
	a = AI++;
	a->setName("a");
	llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", func);
	builder.SetInsertPoint(entry);
	a = builder.CreateVectorSplat(vector_type->getNumElements(), a, "a");
	builder.CreateRet(emit(tree->root));
}

inline void parser::jit::impl::link()
{
//	module->dump();
#if 0
	std::string error;
	if (llvm::verifyModule(*module, llvm::ReturnStatusAction, &error)) {
		std::cerr << error << std::endl;
		abort();
	}
#endif

	pass_man.run(*module);
	engine->finalizeObject();
//	module->dump();
#if 0
	std::string error;
	llvm::raw_fd_ostream dest("module.ll", error);
	if (!error.empty()) {
		std::cerr << error << std::endl;
		abort();
	}
	module->print(dest, 0);
	if (!error.empty()) {
		std::cerr << error << std::endl;
		abort();
	}
	dest.flush();
#endif
}

inline void *parser::jit::impl::func(const char *name)
{
	llvm::Function *func = engine->FindFunctionNamed(name);
	if (!func) {
		std::cerr << "LLVMFindFunction: function \"" << name << "\" not found" << std::endl;
		abort();
	}
	return engine->getPointerToFunction(func);
}

inline parser::jit::impl::~impl()
{
	delete bitcode;
	delete engine;
//	delete module; engine owns module ..
}

void parser::jit::reset() { impl->reset(); }
parser::jit::~jit() { delete impl; }
void *parser::jit::func(const char *name) { return impl->func(name); }
void parser::jit::link() { impl->link(); }
void parser::jit::build(struct parser::tree *tree, const char *name) { impl->build(tree, name); }
parser::jit::jit(char *code, int len) { impl = new struct impl(code, len); }

