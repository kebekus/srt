cmake_minimum_required(VERSION 2.8.6)

project(libsrt)

set(CMAKE_BUILD_TYPE Debug)

find_program(CLANG_EXECUTABLE NAMES clang DOC "Path to clang.")
if (NOT CLANG_EXECUTABLE)
	message(FATAL_ERROR "clang not found")
endif()

find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config-3.4 llvm-config-3.3 llvm-config DOC "Path to llvm-config.")
if (NOT LLVM_CONFIG_EXECUTABLE)
	message(FATAL_ERROR "llvm-config not found.")
endif()

execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
	OUTPUT_VARIABLE LLVM_LDFLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs ipo mcjit native bitreader
	OUTPUT_VARIABLE LLVM_LIBS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
	COMMAND ${LLVM_CONFIG_EXECUTABLE} --cxxflags
	OUTPUT_VARIABLE LLVM_CXXFLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_custom_command(
	OUTPUT value_bc.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND clang -o value.bc ${CMAKE_CURRENT_SOURCE_DIR}/value.c
		-c -std=c99 -W -Wall -Wextra -Wno-unused-function -O3 -fPIC -emit-llvm
	COMMAND xxd -i value.bc value_bc.h
	DEPENDS value.c
)

set_property(SOURCE jit.cpp PROPERTY COMPILE_FLAGS ${LLVM_CXXFLAGS})

add_library(srt SHARED
	copy.cpp
	deriv.cpp
	error.cpp
	eval.cpp
	jit.cpp
	parser.cpp
	reduce.cpp
	value_bc.h
)

target_link_libraries(srt ${LLVM_LDFLAGS} ${LLVM_LIBS})
INSTALL(TARGETS srt DESTINATION lib)

#
# CMake config file
#
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/srtConfig.cmake DESTINATION lib/srt)
