
CC = $(shell ls /usr/bin/gcc-*.*.* | tail -n1)
CFLAGS = -march=native -std=gnu99 -Wall -W -O3 -Wextra -fno-math-errno -ffinite-math-only -fno-rounding-math -fno-signaling-nans -fno-trapping-math -fcx-limited-range -fsingle-precision-constant -g
LDFLAGS = -lm
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LDFLAGS = $(shell sdl-config --libs)

WIN32_CC = i686-mingw32-gcc
WIN32_CFLAGS = -I../SDL-1.2.15/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main -std=gnu99 -Wall -W -O3 -Wextra -fno-math-errno -ffinite-math-only -fno-rounding-math -fno-signaling-nans -fno-trapping-math -fcx-limited-range -fsingle-precision-constant -msse -msse2 -mfpmath=sse
WIN32_LDFLAGS = -L../SDL-1.2.15/lib -lmingw32 -lSDLmain -lSDL -mwindows

all: srt
win: win32/srt.exe

test: srt
	./srt

srt: srt.c curves/heart.so curves/sphere.so curves/torus.so *.h Makefile
	$(CC) -o srt srt.c $(SDL_CFLAGS) $(CFLAGS) $(SDL_LDFLAGS) $(LDFLAGS)

curves/heart.so: value.c curves/heart.h *.h Makefile
	$(CC) -o curves/heart.so -I. -DCURVE="curves/heart.h" value.c -shared -fPIC $(CFLAGS) $(LDFLAGS)

curves/sphere.so: value.c curves/sphere.h *.h Makefile
	$(CC) -o curves/sphere.so -I. -DCURVE="curves/sphere.h" value.c -shared -fPIC $(CFLAGS) $(LDFLAGS)

curves/torus.so: value.c curves/torus.h *.h Makefile
	$(CC) -o curves/torus.so -I. -DCURVE="curves/torus.h" value.c -shared -fPIC $(CFLAGS) $(LDFLAGS)

win32/srt.exe: srt.c *.h Makefile
	$(WIN32_CC) -o win32/srt.exe srt.c $(WIN32_CFLAGS) $(WIN32_LDFLAGS)

clean:
	rm -f srt win32/srt.exe win32/*.txt curves/*.so

