#! /bin/bash
rm -rf build/linux
mkdir -p build/linux
cd build/linux

export CC=clang
export CXX=clang++
export Qt5Core_DIR=$HOME/software/buildsystems/Qt/5.1.0/gcc_64/lib/cmake/Qt5Core

cmake ../../src/
make
