
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


#include <math.h>
#include <stdlib.h>
#include <SDL.h>
#include <unistd.h>

#include "edit.h"
#include "ppm.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "sphere.h"
#include "aabb.h"
#include "camera.h"
#include "parser.h"
#include "deriv.h"
#include "error.h"
#include "reduce.h"
#include "stripe_data.h"
#include "value_bc.h"
#include "jit.h"

int64_t (*stripe)(struct stripe_data *sd, int j);
v4sf (*curve)(m34sf v, float a);

int jit_curve(struct edit *edit)
{
	static class srt::jit *jit;
	static struct srt::tree *curve_tree;
	static struct srt::tree *deriv_tree[3];

	static int init = 0;
	if (!init) {
		jit = new srt::jit((char *)value_bc, value_bc_len);
		curve_tree = srt::alloc_tree(8192);
		for (int j = 0; j < 3; j++)
			deriv_tree[j] = srt::alloc_tree(8192);
		init = 1;
	}

	if (!srt::parse(curve_tree, edit->str)) {
		edit_msg(edit, srt::error::get_str(), srt::error::get_pos());
		return 0;
	}
	if (!srt::reduce(curve_tree)) {
		fprintf(stderr, "%s\n", srt::error::get_str());
		return 0;
	}
	for (int j = 0; j < 3; j++) {
		if (!srt::deriv(deriv_tree[j], curve_tree, srt::token_x + j) || !srt::reduce(deriv_tree[j])) {
			fprintf(stderr, "%s\n", srt::error::get_str());
			return 0;
		}
	}
	jit->reset();
	jit->build(curve_tree, "curve_xyza");
	jit->build(deriv_tree[0], "deriv_x");
	jit->build(deriv_tree[1], "deriv_y");
	jit->build(deriv_tree[2], "deriv_z");

	jit->link();
	stripe = (int64_t (*)(struct stripe_data *, int))jit->func("stripe");
	curve = (v4sf (*)(m34sf, float))jit->func("curve");

	edit_msg(edit, 0, 0);

	return 1;
}

void snapshot(SDL_Surface *screen, struct camera camera, float a, int i, int j)
{
	int w = screen->w;
	int h = screen->h;
	float dw = 2.0f / (float)h;
	float dh = -2.0f / (float)h;
	float ow = (float)w / (float)h;
	float oh = 1;
	if (w < h) {
		dw = 2.0f / (float)w;
		dh = -2.0f / (float)w;
		ow = 1;
		oh = (float)h / (float)w;
	}
	v4sf U = v4sf_set1(i * dw) * camera.right - v4sf_set1(ow) * camera.right;
	v4sf V = v4sf_set1(j * dh) * camera.up + v4sf_set1(oh) * camera.up;
	v4sf dir = v4sf_normal3(U + V + camera.dir);
	struct ray ray = init_ray(m34sf_splat(camera.origin), m34sf_splat(dir));
	FILE *file = fopen("plot.dat", "w");
	for (float x = 0; x < 10; x += 0.1)
		fprintf(file, "%g %g NaN\n", x, curve(ray_point(v4sf_set1(x), ray), a)[0]);
	for (float x = 0; x < 10; x += 0.001)
		fprintf(file, "%g NaN %g\n", x, curve(ray_point(v4sf_set1(x), ray), a)[0]);
	fclose(file);
}

void handle_events(SDL_Surface **screen, struct camera *camera, float *a, struct edit *edit, int *edit_mode, int *use_aabb)
{
	static int focus = 1;
	static int button_left = 0;
	static int button_right = 0;
	int width = 0;
	int height = 0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (*edit_mode && handle_edit(event, edit))
			continue;
		switch (event.type) {
			case SDL_ACTIVEEVENT:
				focus = event.active.gain;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_e:
						*edit_mode = 1;
						break;
					case SDLK_RETURN:
						if (*edit_mode && jit_curve(edit))
							*edit_mode = 0;
						break;
					case SDLK_ESCAPE:
						if (*edit_mode) {
							*edit_mode = 0;
							break;
						}
						exit(0);
						break;
					case SDLK_EQUALS:
						*a = (*a + 0.01) < 1 ? (*a + 0.01) : 1;
						break;
					case SDLK_MINUS:
						*a = (*a - 0.01) > 0 ? (*a - 0.01) : 0;
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
					case SDLK_p:
						ppm_save("screenshot.ppm", *screen);
						break;
					case SDLK_b:
						*use_aabb ^= 1;
						break;
					case SDLK_1:
						reset_edit(edit, "(2*x^2 + y^2 + z^2 - 1)^3 - x^2*z^3/10 - y^2*z^3");
						jit_curve(edit);
						break;
					case SDLK_2:
						reset_edit(edit, "x^2 + 4 * y^2 + z^2 - 8");
						jit_curve(edit);
						break;
					case SDLK_3:
						reset_edit(edit, "(x^2 + y^2 + z^2 + 2)^2 - 12*(x^2 + y^2)");
						jit_curve(edit);
						break;
					case SDLK_4:
						reset_edit(edit, "x*y*z");
						jit_curve(edit);
						break;
					case SDLK_5:
						reset_edit(edit, "x^2 + y^2 + z - 1");
						jit_curve(edit);
						break;
					case SDLK_6:
						reset_edit(edit, "x^2 + y^2 - 1");
						jit_curve(edit);
						break;
					case SDLK_7:
						reset_edit(edit, "x^2 + y^2 - z^2");
						jit_curve(edit);
						break;
					case SDLK_8:
						reset_edit(edit, "4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
						jit_curve(edit);
						break;
					case SDLK_q:
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
						snapshot(*screen, *camera, *a, event.button.x, event.button.y);
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
					m4sf pitch = m4sf_rot(camera->right, M_PI * (float)event.motion.yrel / (float)(*screen)->h);
					m4sf yaw = m4sf_rot(camera->up, -M_PI * (float)event.motion.xrel / (float)(*screen)->w);
					camera->up = m4sf_vmul(pitch, camera->up);
					camera->right = m4sf_vmul(yaw, camera->right);
					camera->dir = v4sf_cross3(camera->up, camera->right);
				}
				if (button_right) {
					m4sf rotx = m4sf_rot(camera->right, -M_PI * (float)event.motion.yrel / (float)(*screen)->h);
					m4sf roty = m4sf_rot(camera->up, -M_PI * (float)event.motion.xrel / (float)(*screen)->w);
					m4sf rotation = m4sf_mul(rotx, roty);
					camera->origin = m4sf_vmul(rotation, camera->origin);
					camera->up = m4sf_vmul(rotation, camera->up);
					camera->right = m4sf_vmul(rotation, camera->right);
					camera->dir = v4sf_cross3(camera->up, camera->right);
				}
				break;
			case SDL_VIDEORESIZE:
				width = event.resize.w & ~1;
				height = event.resize.h & ~1;
				break;
			case SDL_QUIT:
				exit(0);
				break;
			default:
				break;
		}
	}
	if (width && height) {
		*screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF|SDL_RESIZABLE);
		if (!*screen)
			exit(1);
		if ((*screen)->format->BytesPerPixel != 4)
			exit(1);
		resize_edit(edit, 10, (3 * (*screen)->h) / 4, (*screen)->w - 10, (*screen)->h - 10);
	}
	if (!focus)
		SDL_Delay(100);

}

void handle_stats(int64_t *pixels)
{
	static int64_t last;
	static int frames;
	int64_t ticks = SDL_GetTicks();
	frames++;
	if (abs(ticks - last) > 1000) {
		char tmp[100];
		float mpps = 0.001 * (float)(*pixels / (ticks - last));
		int fps = (1000 * frames) / (ticks - last);
		snprintf(tmp, 100, "SIMD Ray Tracing - %g mp/s, %d f/s", mpps, fps);
		SDL_WM_SetCaption(tmp, "srt");
		last = ticks;
		*pixels = 0;
		frames = 0;
	}
}

struct thread_data {
	struct stripe_data sd;
	SDL_mutex *mutex;
	SDL_cond *begin, *done;
	int pause;
	int busy;
	int stripe;
	int height;
	int64_t pixels;
};

int thread(void *data)
{
	struct thread_data *td = (struct thread_data *)data;
	SDL_mutexP(td->mutex);
	(td->busy)++;
	while (td->pause)
		SDL_CondWait(td->begin, td->mutex);
	SDL_mutexV(td->mutex);
	int64_t pixels = 0;
	while (1) {
		SDL_mutexP(td->mutex);
		td->pixels += pixels;
		(td->busy)--;
		if (td->stripe >= td->height)
			SDL_CondSignal(td->done);
		while (td->stripe >= td->height)
			SDL_CondWait(td->begin, td->mutex);
		int j = td->stripe;
		td->stripe += 2;
		(td->busy)++;
		SDL_mutexV(td->mutex);
		pixels = stripe(&td->sd, j);
	}
}

void draw(SDL_Surface *screen, struct thread_data *td, struct camera camera, float a, int use_aabb)
{
	struct sphere sphere = { v4sf_set3(0, 0, 0), 3 };
	struct aabb aabb = { v4sf_set3(-3, -3, -3), v4sf_set3(3, 3, 3) };
	uint32_t *fb = (uint32_t *)screen->pixels;
	int w = screen->w;
	int h = screen->h;
	float dw = 2.0f / (float)h;
	float dh = -2.0f / (float)h;
	float ow = (float)w / (float)h;
	float oh = 1;
	if (w < h) {
		dw = 2.0f / (float)w;
		dh = -2.0f / (float)w;
		ow = 1;
		oh = (float)h / (float)w;
	}
	v4sf dU = v4sf_set1(dw) * camera.right;
	v4sf dV = v4sf_set1(dh) * camera.up;
	v4sf oU = v4sf_set1(ow) * camera.right;
	v4sf oV = v4sf_set1(oh) * camera.up;
	m34sf zU = m34sf_set(v4sf_set1(0), dU, v4sf_set1(0), dU);
	m34sf zV = m34sf_set(v4sf_set1(0), v4sf_set1(0), dV, dV);
	m34sf U = m34sf_subv(zU, oU);
	m34sf V = m34sf_addv(zV, oV);
	m34sf UV = m34sf_add(U, V);
	td->sd = (struct stripe_data){ fb, w, h, dU, dV, UV, sphere, aabb, camera, a, use_aabb };
	td->height = h;
	td->stripe = 0;
	td->pause = 0;
	SDL_CondBroadcast(td->begin);
	while (td->busy || td->stripe < td->height)
		SDL_CondWait(td->done, td->mutex);
}

int main(int argc, char **argv)
{
	const char *str = "4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2";
	if (argc == 2)
		str = argv[1];
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(512, 512, 32, SDL_DOUBLEBUF|SDL_RESIZABLE);
	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	SDL_WM_SetCaption("SIMD Ray Tracing", "srt");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableUNICODE(1);

	int threads = sysconf(_SC_NPROCESSORS_ONLN);
	fprintf(stderr, "using %d threads\n", threads);
	struct thread_data td;
	td.mutex = SDL_CreateMutex();
	SDL_mutexP(td.mutex);
	td.begin = SDL_CreateCond();
	td.done = SDL_CreateCond();
	td.pause = 1;
	td.busy = 0;
	td.pixels = 0;
	for (int i = 0; i < threads; i++)
		SDL_CreateThread(thread, &td);

	struct edit *edit = alloc_edit(10240, "");
	resize_edit(edit, 10, (3 * screen->h) / 4, screen->w - 10, screen->h - 10);

	struct camera camera = init_camera();
	reset_edit(edit, str);
	if (!jit_curve(edit)) {
		fprintf(stderr, "\n%s\n", str);
		for (int i = 0; i < srt::error::get_pos(); i++)
			fprintf(stderr, " ");
		fprintf(stderr, "~ %s\n\n", srt::error::get_str());
		exit(1);
	}
	float a = 1.0;

	int edit_mode = 0;
	int use_aabb = 0;
	for (;;) {
		draw(screen, &td, camera, a, use_aabb);
		if (edit_mode)
			draw_edit(edit, screen, 0x00bebebe, 0);
		SDL_Flip(screen);
		handle_events(&screen, &camera, &a, edit, &edit_mode, &use_aabb);
		handle_stats(&td.pixels);
	}
	return 0;
}

