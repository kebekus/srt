
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <dlfcn.h>
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

int mouse_x = 0;
int mouse_y = 0;
int snap = 0;

v4sf (*value)(float l[2], struct ray ray);
float (*curve)(v4sf v);

void load_curve(char *name)
{
	static void *lh = 0;
	if (lh)
		dlclose(lh);
	lh = dlopen(name, RTLD_LAZY);
	if (!lh) {
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}
	value = dlsym(lh, "value");
	if (!value) {
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}
	curve = dlsym(lh, "curve");
	if (!curve) {
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}
}

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
					case SDLK_1:
						load_curve("curves/heart.so");
						break;
					case SDLK_2:
						load_curve("curves/mnm.so");
						break;
					case SDLK_3:
						load_curve("curves/torus.so");
						break;
					case SDLK_4:
						load_curve("curves/cross.so");
						break;
					case SDLK_5:
						load_curve("curves/paraboloid.so");
						break;
					case SDLK_6:
						load_curve("curves/cylinder.so");
						break;
					case SDLK_7:
						load_curve("curves/cone.so");
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
					case SDL_BUTTON_MIDDLE:
						mouse_x = event.button.x;
						mouse_y = event.button.y;
						snap = 1;
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

void draw(SDL_Surface *screen, struct camera camera)
{
	struct aabb aabb = { v4sf_set3(-1.5, -1.5, -1.5), v4sf_set3(1.5, 1.5, 1.5) };
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
			float l[2];
			uint32_t color = 0;
			if (aabb_ray(l, aabb, ray) && l[1] > 0) {
				l[0] = fmaxf(l[0], 0);
				if (snap && mouse_y == j && mouse_x == i) {
					snap = 0;
					FILE *file = fopen("plot.dat", "w");
					for (float x = l[0]; x < l[1]; x += 0.1)
						fprintf(file, "%g %g NaN\n", x, curve(ray.o + v4sf_set1(x) * ray.d));
					for (float x = l[0]; x < l[1]; x += 0.001)
						fprintf(file, "%g NaN %g\n", x, curve(ray.o + v4sf_set1(x) * ray.d));
					fclose(file);
				}
				color = argb(value(l, ray));
			}
			fb[w * j + i] = color;
		}
	}
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
	load_curve("curves/heart.so");

	for (;;) {
		draw(screen, camera);
		SDL_Flip(screen);
		SDL_Delay(10);
		handle_events(screen, &camera);
		handle_stats(screen);
	}
	return 0;
}

