
#CC = clang
#CXX = clang++

CC = $(notdir $(shell ls /usr/bin/gcc-*.*.* | tail -n1))
CXX = $(notdir $(shell ls /usr/bin/g++-*.*.* | tail -n1))

#OPT = -march=native -msse4.1
#OPT = -march=native -mavx
OPT = -march=native

STD = -std=c99
CFLAGS = -W -Wall -Wextra -O3 -fPIC
LIBS = -lm -lpthread -ldl
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LIBS = $(shell sdl-config --libs)

all: srt

test: srt
	./srt

srt: srt.o parser.o deriv.o error.o eval.o reduce.o copy.o gcc.o edit.o ppm.o
	$(CC) -o $@ $^ $(LIBS) $(SDL_LIBS)

srt.o: srt.c value.o *.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(OPT) $(SDL_CFLAGS)

ppm.o: ppm.c ppm.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(SDL_CFLAGS)

edit.o: edit.c edit.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(SDL_CFLAGS)

gcc.o: gcc.c jit.h parser.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) -DCC="\"$(CC)\"" -DCFLAGS="\"$(STD) $(CFLAGS) $(OPT)\""

value.o: value.c ray.h vector.h scalar.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS) $(OPT)

%.o: %.c parser.h Makefile
	$(CC) -o $@ $< -c $(STD) $(CFLAGS)

clean:
	rm -f srt module.c module.so *.o *.so

