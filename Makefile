
CC = clang
CXX = clang++

#CC = $(notdir $(shell ls /usr/bin/gcc-*.*.* | tail -n1))
#CXX = $(notdir $(shell ls /usr/bin/g++-*.*.* | tail -n1))
#OPENMP = -fopenmp

STD = -std=c99
CFLAGS = -W -Wall -Wextra -O3 -march=native -ffast-math -fPIC
LIBS = -lm
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LIBS = $(shell sdl-config --libs)
LLVM_CFLAGS = $(shell llvm-config --cflags)
LLVM_LIBS = $(shell llvm-config --libs --ldflags ipo jit native bitreader) -lstdc++

all: srt

test: srt
	./srt

srt: srt.o parser.o deriv.o error.o eval.o reduce.o copy.o cbind.o jit.o edit.o
	$(CC) -o $@ $^ $(OPENMP) $(LIBS) $(LLVM_LIBS) $(SDL_LIBS)

srt.o: srt.c value_bc.h *.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(SDL_CFLAGS) $(OPENMP)

edit.o: edit.c edit.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(SDL_CFLAGS)

cbind.o: cbind.cpp cbind.h Makefile
	$(CXX) -o $@ $< -c $(CFLAGS) $(LLVM_CFLAGS)

jit.o: jit.c jit.h parser.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(LLVM_CFLAGS)

value.bc: value.c ray.h vector.h scalar.h Makefile
	clang -o $@ $< -c $(STD) $(CFLAGS) -emit-llvm

value_bc.h: value.bc Makefile
	xxd -i value.bc value_bc.h

%.o: %.c parser.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS)

clean:
	rm -f srt *.o *_bc.h *.bc

