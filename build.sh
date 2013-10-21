#! /bin/bash
rm -rf build/linux
mkdir -p build/linux
cd build/linux

export CC=gcc-4.8
export CXX=g++-4.8
export Qt5Core_DIR=$HOME/software/buildsystems/Qt/5.1.1/gcc/lib/cmake/Qt5Core
export Qt5Designer_DIR=$HOME/software/buildsystems/Qt/5.1.1/gcc/lib/cmake/Qt5Designer

cmake ../../src -DCXX_FLAGS="-march=native"
make
