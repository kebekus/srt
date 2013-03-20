
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <math.h>
#include <stdlib.h>
#include <SDL.h>
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "aabb.h"
#include "camera.h"
#include "utils.h"

void handle_events()
{
	static int focus = 1;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_ACTIVEEVENT:
				focus = event.active.gain;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_q:
					case SDLK_ESCAPE:
						exit(0);
						break;
					default:
						break;
				}
				break;
			case SDL_QUIT:
				exit(0);
				break;
			default:
				break;
		}
	}
	if (!focus)
		SDL_Delay(100);

}

void handle_stats(SDL_Surface *screen)
{
	static int64_t pixels;
	static int64_t last;
	int64_t ticks = SDL_GetTicks();
	pixels += screen->w * screen->h;
	if (abs(ticks - last) > 1000) {
		char tmp[100];
		float mpps = 0.001 * (float)(pixels / (ticks - last));
		snprintf(tmp, 100, "SIMD Ray Tracing - %g mega pixels / second", mpps);
		SDL_WM_SetCaption(tmp, "srt");
		last = ticks;
		pixels = 0;
	}
}

uint32_t argb(int r, int g, int b)
{
	return ((r & 255) << 16) | ((g & 255) << 8) | ((b & 255) << 0);
}

void draw(SDL_Surface *screen)
{
	uint32_t *fb = screen->pixels;
	int w = screen->w;
	int h = screen->h;
	for (int j = 0; j < h; j++)
		for (int i = 0; i < w; i++)
			fb[w * j + i] = argb(i, j, 0);
}

void test()
{
	v4sf V = { 1, 2, 3, 4 };
	v4sf_print_name(V, "V");
	m4sf A = {
		{ 1, 2, 3, 4 },
		{ 5, 6, 7, 8 },
		{ 9, 10, 11, 12 },
		{ 13, 14, 15, 16 }
	};
	m4sf_print_name(A, "A");
	m4sf B = {
		{ 17, 18, 19, 20 },
		{ 21, 22, 23, 24 },
		{ 25, 26, 27, 28 },
		{ 29, 30, 31, 32 }
	};
	m4sf_print_name(B, "B");
	m4sf_print_name(m4sf_mul(A, B), "A * B");
}

int main(int argc, char **argv)
{
	(void)argc; (void)argv;
	atexit(SDL_Quit);
	test();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(1024, 1024, 32, SDL_DOUBLEBUF);
	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	SDL_WM_SetCaption("SIMD Ray Tracing", "srt");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	for (;;) {
		draw(screen);
		SDL_Flip(screen);
		SDL_Delay(10);
		handle_events();
		handle_stats(screen);
	}
	return 0;
}

