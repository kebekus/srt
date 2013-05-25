#include <llvm/Support/MemoryBuffer.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "cbind.h"

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

