#ifndef CBIND_H
#define CBIND_H

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#ifdef __cplusplus
extern "C" {
#endif

LLVMMemoryBufferRef LLVMGetMemoryBufferFromArray(const char *Array, unsigned Length);
void *LLVMGetPointerToFunction(LLVMExecutionEngineRef EE, LLVMValueRef F);

#ifdef __cplusplus
}
#endif

#endif
