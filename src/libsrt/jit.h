
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef JIT_H
#define JIT_H
#include "parser.h"
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
namespace parser
{
	class jit
	{
	public:
		jit(char *code, int len);
		~jit();
		void reset();
		void build(struct parser_tree *tree, const char *name);
		void *func(const char *name);
		void link();
	private:
		LLVMModuleRef module;
		LLVMBuilderRef builder;
		LLVMPassManagerRef pass;
		LLVMExecutionEngineRef engine;
		LLVMMemoryBufferRef bc;
	};
}
#endif
