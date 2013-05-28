
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
#include "sphere.h"
#include "camera.h"
#include "utils.h"
#include "tests.h"

#include "parser.h"
#include "deriv.h"
#include "error.h"
#include "reduce.h"
#include "jit.h"
#include "value_bc.h"

int mouse_x = 0;
int mouse_y = 0;
int snap = 0;

v4sf (*value)(float l[2], struct ray ray, float A);
float (*curve)(v4sf v, float A);

char *curve_str = "";

int jit_curve(char *str)
{
	static struct parser_jit *jit;
	static struct parser_tree *curve_tree;
	static struct parser_tree *deriv_tree[3];

	static int init = 0;
	if (!init) {
		jit = parser_alloc_jit((char *)value_bc, value_bc_len);
		curve_tree = parser_alloc_tree(8192);
		for (int j = 0; j < 3; j++)
			deriv_tree[j] = parser_alloc_tree(8192);
		init = 1;
	}

	if (!parser_parse(curve_tree, str)) {
		fprintf(stderr, "\n%s\n", str);
		for (int i = 0; i < get_err_pos(); i++)
			fprintf(stderr, " ");
		fprintf(stderr, "~ %s\n\n", get_err_str());
		return 0;
	}
	if (!parser_reduce(curve_tree)) {
		fprintf(stderr, "%s\n", get_err_str());
		return 0;
	}
	for (int j = 0; j < 3; j++) {
		if (!parser_deriv(deriv_tree[j], curve_tree, token_x + j) || !parser_reduce(deriv_tree[j])) {
			fprintf(stderr, "%s\n", get_err_str());
			return 0;
		}
	}
	parser_reset_jit(jit);
	parser_jit_build(jit, curve_tree, "curve_xyza");
	parser_jit_build(jit, deriv_tree[0], "deriv_x");
	parser_jit_build(jit, deriv_tree[1], "deriv_y");
	parser_jit_build(jit, deriv_tree[2], "deriv_z");

	parser_jit_opt(jit);
	value = parser_jit_func(jit, "value");
	curve = parser_jit_func(jit, "curve");

	curve_str = str;
	return 1;
}

void draw_string(char *string, SDL_Surface *screen, int x0, int y0, int x1, int y1, uint32_t fg, uint32_t bg)
{
#include "ascii.h"
	const int font_w = 10;
	const int font_h = 20;
	uint32_t *fbp = (uint32_t *)screen->pixels;
	int w = screen->w;
	int h = fminf(screen->h, y1);
	int yoff = 0;
	int xoff = 0;
	for (int i = 0; string[i]; i++) {
		if ('\n' == string[i]) {
			yoff += font_h;
			xoff = 0;
			continue;
		}
		if (string[i] < 32 || 126 < string[i]) continue;
		int c = (string[i] - 32) * font_w * font_h;
		for (int j = 0; j < font_h; j++) {
			for (int k = 0; k < font_w; k++) {
				int bit = c + font_w * j + k;
				uint32_t mask = 1 << (bit & 31);
				int index = bit >> 5;
				int x = k + xoff + x0;
				int y = j + yoff + y0;
				if (w <= x || h <= y) continue;
				fbp[w * y + x] = (mask & ascii[index]) ? fg : bg;
			}
		}
		xoff += font_w;
		if (x1 <= (xoff + x0 + font_w)) {
			yoff += font_h;
			xoff = 0;
		}
	}
}

void handle_events(SDL_Surface *screen, struct camera *camera, float *A)
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
					case SDLK_EQUALS:
						*A = (*A + 0.01) < 1 ? (*A + 0.01) : 1;
						break;
					case SDLK_MINUS:
						*A = (*A - 0.01) > 0 ? (*A - 0.01) : 0;
						break;
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
						jit_curve("(2*x^2 + y^2 + z^2 - 1)^3 - x^2*z^3/10 - y^2*z^3");
						break;
					case SDLK_2:
						jit_curve("x^2 + 4 * y^2 + z^2 - 1");
						break;
					case SDLK_3:
						jit_curve("(x^2 + y^2 + z^2 + 0.2)^2 - (x^2 + y^2)");
						break;
					case SDLK_4:
						jit_curve("x*y*z");
						break;
					case SDLK_5:
						jit_curve("x^2 + y^2 + z - 1");
						break;
					case SDLK_6:
						jit_curve("x^2 + y^2 - 1");
						break;
					case SDLK_7:
						jit_curve("x^2 + y^2 - z^2");
						break;
					case SDLK_8:
						jit_curve("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
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

void draw(SDL_Surface *screen, struct camera camera, float A)
{
	struct sphere sphere = { v4sf_set3(0, 0, 0), 3 };
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
			if (sphere_ray(l, sphere, ray) && l[1] > 0) {
				l[0] = fmaxf(l[0], 0);
				if (snap && mouse_y == j && mouse_x == i) {
					snap = 0;
					FILE *file = fopen("plot.dat", "w");
					for (float x = l[0]; x < l[1]; x += 0.1)
						fprintf(file, "%g %g NaN\n", x, curve(ray.o + v4sf_set1(x) * ray.d, A));
					for (float x = l[0]; x < l[1]; x += 0.001)
						fprintf(file, "%g NaN %g\n", x, curve(ray.o + v4sf_set1(x) * ray.d, A));
					fclose(file);
				}
				color = argb(value(l, ray, A));
			}
			fb[w * j + i] = color;
		}
	}
	draw_string(curve_str, screen, 10, (3 * h) / 4, w - 10, h - 10, 0x00bebebe, 0);
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
	jit_curve("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
	float A = 1.0;

	for (;;) {
		draw(screen, camera, A);
		SDL_Flip(screen);
		SDL_Delay(10);
		handle_events(screen, &camera, &A);
		handle_stats(screen);
	}
	return 0;
}

