cmake_minimum_required(VERSION 2.8.6)

#
# External packages
#

# SDL
Find_Package(SDL REQUIRED)

# CLANG
find_program(
  CLANG_EXECUTABLE NAMES clang DOC "Path to the clang front-end."
  )
if(NOT CLANG_EXECUTABLE)
  message(FATAL_ERROR "clang not found")
endif()

# LLVM
find_program(
  LLVM_CONFIG_EXECUTABLE NAMES llvm-config DOC "Path to llvm-config."
  )
if(NOT LLVM_CONFIG_EXECUTABLE)
  message(FATAL_ERROR "The programm 'llvm-config' could not be found. Aborting.")
endif()
execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs ipo jit native bitreader
  OUTPUT_VARIABLE LLVM_LIBS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
STRING(REGEX REPLACE "-l" "" LLVM_LIBS "${LLVM_LIBS}" )
STRING(REGEX REPLACE " " ";" LLVM_LIBS "${LLVM_LIBS}" )
execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
  OUTPUT_VARIABLE LLVM_LDFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# Qt
FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtTest)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Doxygen for API documentation
find_package(Doxygen REQUIRED)

#
# General compiler settings
#
add_definitions( -msse2 -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS )



#
# Add subprojects
#
add_subdirectory(examples)
add_subdirectory(libsrt)
add_subdirectory(libqsrt)
add_subdirectory(srtTestSDL)
add_subdirectory(srtTestQt)
