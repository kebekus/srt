project(libsrt)


add_definitions( -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -DUSE_LLVM )
SET( CMAKE_C_FLAGS  "-std=c99 -march=native" )


add_custom_command(
  OUTPUT value_bc.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND clang -o value.bc ${CMAKE_CURRENT_SOURCE_DIR}/value.c -c -std=c99 -W -Wall -Wextra -O3 -fPIC -DUSE_LLVM -emit-llvm
  COMMAND xxd -i value.bc value_bc.h
  DEPENDS value.c
  )

add_library(srt SHARED
  copy.c
  deriv.c
  error.c
  eval.c
  cbind.cpp
  jit.c
  parser.c
  reduce.c
  value_bc.h
)
target_link_libraries( srt ${LLVM_LIBS} ${LLVM_LDFLAGS} )
