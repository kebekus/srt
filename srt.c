
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
#include "tests.h"

void handle_events(SDL_Surface *screen, struct camera *camera)
{
	static int focus = 1;
	static int button_left = 0;
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
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button) {
					case SDL_BUTTON_LEFT:
						button_left = 1;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button) {
					case SDL_BUTTON_LEFT:
						button_left = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (button_left) {
					m4sf pitch = m4sf_rot((v4sf){1, 0, 0, 0}, M_PI * (float)event.motion.yrel / (float)screen->h);
					m4sf yaw = m4sf_rot((v4sf){0, 1, 0, 0}, -M_PI * (float)event.motion.xrel / (float)screen->w);
					camera->up = m4sf_vmul(pitch, camera->up);
					camera->right = m4sf_vmul(yaw, camera->right);
					camera->dir = v4sf_cross3(camera->up, camera->right);
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

uint32_t argb(v4sf c)
{
	int r = fmaxf(fminf(255 * c[0], 255), 0);
	int g = fmaxf(fminf(255 * c[1], 255), 0);
	int b = fmaxf(fminf(255 * c[2], 255), 0);
	return ((r & 255) << 16) | ((g & 255) << 8) | ((b & 255) << 0);
}

void draw(SDL_Surface *screen, struct camera camera)
{
	struct aabb aabb = {
		{ -1, -1, -1, 0 },
		{ 1, 1, 1, 0 }
	};
	uint32_t *fb = screen->pixels;
	int w = screen->w;
	int h = screen->h;
	v4sf dU = v4sf_set1(2) * camera.right / v4sf_set1(w);
	v4sf dV = - v4sf_set1(2) * camera.up / v4sf_set1(h);
	v4sf V = camera.up;
	for (int j = 0; j < h; j++, V += dV) {
		v4sf U = - camera.right;
		for (int i = 0; i < w; i++, U += dU) {
			v4sf dir = v4sf_normal3(U + V + camera.dir);
			struct ray ray = init_ray(camera.origin, dir);
			fb[w * j + i] = 0;
			v4sf l[2];
			if (aabb_ray(l, aabb, ray))
				fb[w * j + i] = argb((v4sf){ 1, 1, 1, 0 });

		}
	}
}

int main(int argc, char **argv)
{
	(void)argc; (void)argv;
	atexit(SDL_Quit);
	// matrix_tests();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(1024, 1024, 32, SDL_DOUBLEBUF);
	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	SDL_WM_SetCaption("SIMD Ray Tracing", "srt");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	struct camera camera = init_camera();
	for (;;) {
		draw(screen, camera);
		SDL_Flip(screen);
		SDL_Delay(10);
		handle_events(screen, &camera);
		handle_stats(screen);
	}
	return 0;
}

