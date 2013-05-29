
all: srt

test: srt
	./srt

srt: srt.o parser.o deriv.o error.o eval.o reduce.o copy.o cbind.o jit.o edit.o
	clang -o $@ $^ -lm $(shell llvm-config --libs --ldflags ipo jit native bitreader) -lstdc++ $(shell sdl-config --libs)

srt.o: srt.c value_bc.h *.h Makefile
	clang -o srt.o srt.c -c -std=gnu99 -Wall -W -O3 -Wextra -march=native -ffast-math $(shell sdl-config --cflags)

edit.o: edit.c edit.h Makefile
	clang -o edit.o edit.c -c -std=gnu99 -Wall -W -O3 -Wextra -march=native -ffast-math $(shell sdl-config --cflags)

cbind.o: cbind.cpp cbind.h Makefile
	clang++ -o cbind.o cbind.cpp -c -Wall -W -O3 -Wextra $(shell llvm-config --cflags)

jit.o: jit.c jit.h parser.h Makefile
	clang -o $@ $< -c -std=c99 -W -Wall -Wextra -O3 $(shell llvm-config --cflags)

value.bc: value.c ray.h vector.h scalar.h Makefile
	clang -o value.bc value.c -c -std=gnu99 -Wall -W -O3 -Wextra -fPIC -march=native -ffast-math -emit-llvm

value_bc.h: value.bc Makefile
	xxd -i value.bc value_bc.h

%.o: %.c parser.h Makefile
	clang -o $@ $< -c -std=c99 -W -Wall -Wextra -O3

clean:
	rm -f srt *.o

