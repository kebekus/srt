#! /bin/bash
rm -rf build
mkdir build
cd build
export CC=clang
export CXX=clang++
cmake ../src/
make
