
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
#include "voxel.h"
#include "camera.h"
#include "utils.h"
#include "tests.h"

void handle_events(SDL_Surface *screen, struct camera *camera)
{
	static int focus = 1;
	static int button_left = 0;
	static int button_right = 0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_ACTIVEEVENT:
				focus = event.active.gain;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_w:
						camera->origin += v4sf_set1(0.2) * camera->dir;
						break;
					case SDLK_s:
						camera->origin -= v4sf_set1(0.2) * camera->dir;
						break;
					case SDLK_a:
						camera->origin -= v4sf_set1(0.2) * camera->right;
						break;
					case SDLK_d:
						camera->origin += v4sf_set1(0.2) * camera->right;
						break;
					case SDLK_c:
						camera->origin -= v4sf_set1(0.2) * camera->up;
						break;
					case SDLK_SPACE:
						camera->origin += v4sf_set1(0.2) * camera->up;
						break;
					case SDLK_r:
						*camera = init_camera();
						break;
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
					case SDL_BUTTON_RIGHT:
						button_right = 1;
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
					case SDL_BUTTON_RIGHT:
						button_right = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (button_left) {
					m4sf pitch = m4sf_rot(camera->right, M_PI * (float)event.motion.yrel / (float)screen->h);
					m4sf yaw = m4sf_rot(camera->up, -M_PI * (float)event.motion.xrel / (float)screen->w);
					camera->up = m4sf_vmul(pitch, camera->up);
					camera->right = m4sf_vmul(yaw, camera->right);
					camera->dir = v4sf_cross3(camera->up, camera->right);
				}
				if (button_right) {
					m4sf rotx = m4sf_rot(camera->right, -M_PI * (float)event.motion.yrel / (float)screen->h);
					m4sf roty = m4sf_rot(camera->up, -M_PI * (float)event.motion.xrel / (float)screen->w);
					m4sf rotation = m4sf_mul(rotx, roty);
					camera->origin = m4sf_vmul(rotation, camera->origin);
					camera->up = m4sf_vmul(rotation, camera->up);
					camera->right = m4sf_vmul(rotation, camera->right);
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
	static int frames;
	int64_t ticks = SDL_GetTicks();
	pixels += screen->w * screen->h;
	frames++;
	if (abs(ticks - last) > 1000) {
		char tmp[100];
		float mpps = 0.001 * (float)(pixels / (ticks - last));
		int fps = (1000 * frames) / (ticks - last);
		snprintf(tmp, 100, "SIMD Ray Tracing - %g mp/s, %d f/s", mpps, fps);
		SDL_WM_SetCaption(tmp, "srt");
		last = ticks;
		pixels = 0;
		frames = 0;
	}
}

int value(struct grid *grid, unsigned short *bunny)
{
	int tmp = 0;
	for (first_voxel(grid); inside_grid(grid); next_voxel(grid))
		tmp += bunny[voxel_idx(grid)];
	return tmp;
}

void draw(SDL_Surface *screen, struct camera camera, unsigned short *bunny)
{
	struct aabb aabb = { v4sf_set3(-1, -1, -1), v4sf_set3(1, 1, 1) };
	v4si cells = v4si_set3(512, 512, 361);
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
			struct grid grid;
			uint32_t color = 0;
			if (init_traversal(&grid, ray, aabb, cells))
				color = argb(v4sf_set1((float)value(&grid, bunny) / 10000000));
			fb[w * j + i] = color;

		}
	}
}

void load_bunny(unsigned short *bunny)
{
	for (int i = 0; i < 361; i++) {
		char name[100];
		snprintf(name, 100, "../bunny-ctscan/%d", i+1);
		FILE *f = fopen(name, "rb");
		if (!f) {
			fprintf(stderr, "could not open file \"%s\"\n", name);
			exit(1);
		}
		if (!fread(bunny + 512*512*i, 512*512*2, 1, f)) {
			fprintf(stderr, "could not read file \"%s\"\n", name);
			exit(1);
		}
		fclose(f);
	}
	int min = 65535, max = 0;
	for (int i = 0; i < 361*512*512; i++) {
		min = fminf(min, bunny[i]);
		max = fmaxf(max, bunny[i]);
	}
	printf("min = %d max = %d\n", min, max);
}

int main(int argc, char **argv)
{
	(void)argc; (void)argv;
	atexit(SDL_Quit);
	// matrix_tests();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(512, 512, 32, SDL_DOUBLEBUF);
	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	SDL_WM_SetCaption("SIMD Ray Tracing", "srt");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	struct camera camera = init_camera();
	unsigned short *bunny = malloc(361*512*512*2);
	load_bunny(bunny);

	for (;;) {
		draw(screen, camera, bunny);
		SDL_Flip(screen);
		SDL_Delay(10);
		handle_events(screen, &camera);
		handle_stats(screen);
	}
	return 0;
}

