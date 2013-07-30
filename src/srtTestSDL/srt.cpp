
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


#include <math.h>
#include <stdlib.h>
#include <SDL.h>
#include <pthread.h>
#include <unistd.h>

#include "edit.h"
#include "ppm.h"
extern "C" {
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "sphere.h"
#include "aabb.h"
#include "camera.h"
#include "utils.h"
#include "tests.h"
#include "parser.h"
#include "deriv.h"
#include "error.h"
#include "reduce.h"
#include "jit.h"
#include "stripe_data.h"
#if USE_LLVM
#include "value_bc.h"
#endif
}

#include <QCoreApplication>


int64_t (*stripe)(struct stripe_data *sd, int j);
v4sf (*curve)(m34sf v, float a);

int jit_curve(struct edit *edit)
{
	static struct parser_jit *jit;
	static struct parser_tree *curve_tree;
	static struct parser_tree *deriv_tree[3];

	static int init = 0;
	if (!init) {
#if USE_LLVM
		jit = parser_alloc_jit((char *)value_bc, value_bc_len);
#else
		jit = parser_alloc_jit("value.c", "module.h", "module.so");
#endif
		curve_tree = parser_alloc_tree(8192);
		for (int j = 0; j < 3; j++)
			deriv_tree[j] = parser_alloc_tree(8192);
		init = 1;
	}

	if (!parser_parse(curve_tree, edit->str)) {
		edit_msg(edit, get_err_str(), get_err_pos());
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

	parser_jit_link(jit);
	stripe = (int64_t (*)(struct stripe_data *, int))parser_jit_func(jit, "stripe");
	curve = (v4sf (*)(m34sf, float))parser_jit_func(jit, "curve");

	edit_msg(edit, 0, 0);

	return 1;
}

void snapshot(SDL_Surface *screen, struct camera camera, float a, int i, int j)
{
	int w = screen->w;
	int h = screen->h;
	float dw = 2.0f / (float)w;
	float dh = -2.0f / (float)h;
	v4sf U = v4sf_set1(i * dw) * camera.right - camera.right;
	v4sf V = v4sf_set1(j * dh) * camera.up + camera.up;
	v4sf dir = v4sf_normal3(U + V + camera.dir);
	struct ray ray = init_ray(m34sf_splat(camera.origin), m34sf_splat(dir));
	FILE *file = fopen("plot.dat", "w");
	for (float x = 0; x < 10; x += 0.1)
		fprintf(file, "%g %g NaN\n", x, curve(ray_point(v4sf_set1(x), ray), a)[0]);
	for (float x = 0; x < 10; x += 0.001)
		fprintf(file, "%g NaN %g\n", x, curve(ray_point(v4sf_set1(x), ray), a)[0]);
	fclose(file);
}

void handle_events(SDL_Surface *screen, struct camera *camera, float *a, struct edit *edit, int *edit_mode, int *use_aabb)
{
	static int focus = 1;
	static int button_left = 0;
	static int button_right = 0;
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
						ppm_save("screenshot.ppm", screen);
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
						snapshot(screen, *camera, *a, event.button.x, event.button.y);
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
	struct stripe_data *stripe_data;
	pthread_mutex_t mutex;
	pthread_cond_t begin, done;
	int pause;
	int busy;
	int stripe;
	int height;
	int64_t pixels;
};

void *thread(void *data)
{
	struct thread_data *td = (struct thread_data *)data;
	pthread_mutex_lock(&td->mutex);
	(td->busy)++;
	while (td->pause)
		pthread_cond_wait(&td->begin, &td->mutex);
	pthread_mutex_unlock(&td->mutex);
	int64_t pixels = 0;
	while (1) {
		pthread_mutex_lock(&td->mutex);
		td->pixels += pixels;
		(td->busy)--;
		if (td->stripe >= td->height)
			pthread_cond_signal(&td->done);
		while (td->stripe >= td->height)
			pthread_cond_wait(&td->begin, &td->mutex);
		int j = td->stripe;
		td->stripe += 2;
		(td->busy)++;
		pthread_mutex_unlock(&td->mutex);
		pixels = stripe(td->stripe_data, j);
	}
}

struct stripe_data stripe_data;
struct thread_data thread_data;
void draw(SDL_Surface *screen, struct camera camera, float a, int use_aabb)
{
	struct sphere sphere = { v4sf_set3(0, 0, 0), 3 };
	struct aabb aabb = { v4sf_set3(-3, -3, -3), v4sf_set3(3, 3, 3) };
	uint32_t *fb = (uint32_t *)screen->pixels;
	int w = screen->w;
	int h = screen->h;
	float dw = 2.0f / (float)w;
	float dh = -2.0f / (float)h;
	v4sf dU = v4sf_set1(dw) * camera.right;
	v4sf dV = v4sf_set1(dh) * camera.up;
	m34sf zU = m34sf_set(v4sf_set1(0), dU, v4sf_set1(0), dU);
	m34sf zV = m34sf_set(v4sf_set1(0), v4sf_set1(0), dV, dV);
	m34sf U = m34sf_subv(zU, camera.right);
	m34sf V = m34sf_addv(zV, camera.up);
	m34sf UV = m34sf_add(U, V);
	stripe_data = (struct stripe_data){ fb, w, h, dU, dV, UV, sphere, aabb, camera, a, use_aabb };
	thread_data.height = h;
	thread_data.stripe = 0;
	thread_data.pause = 0;
	pthread_cond_broadcast(&thread_data.begin);
	while (thread_data.busy || thread_data.stripe < thread_data.height)
		pthread_cond_wait(&thread_data.done, &thread_data.mutex);
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("srt");
	app.setOrganizationName("Albert-Ludwigs-Universität Freiburg");
	app.setOrganizationName("uni-freiburg.de");

	const char *str = "4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2";
	if (argc == 2)
		str = argv[1];
	// matrix_tests();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *screen = SDL_SetVideoMode(1024, 1024, 32, SDL_DOUBLEBUF);
	if (!screen)
		exit(1);
	if (screen->format->BytesPerPixel != 4)
		exit(1);
	SDL_WM_SetCaption("SIMD Ray Tracing", "srt");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableUNICODE(1);

	int threads = sysconf(_SC_NPROCESSORS_ONLN);
	fprintf(stderr, "using %d threads\n", threads);
	pthread_t pthd[threads];
	pthread_mutex_init(&thread_data.mutex, 0);
	pthread_mutex_lock(&thread_data.mutex);
	pthread_cond_init(&thread_data.begin, 0);
	pthread_cond_init(&thread_data.done, 0);
	thread_data.pause = 1;
	thread_data.busy = 0;
	thread_data.pixels = 0;
	thread_data.stripe_data = &stripe_data;
	for (int i = 0; i < threads; i++)
		pthread_create(pthd + i, 0, thread, &thread_data);

	struct edit *edit = alloc_edit(10240, "");
	resize_edit(edit, 10, (3 * screen->h) / 4, screen->w - 10, screen->h - 10);

	struct camera camera = init_camera();
	reset_edit(edit, str);
	if (!jit_curve(edit)) {
		fprintf(stderr, "\n%s\n", str);
		for (int i = 0; i < get_err_pos(); i++)
			fprintf(stderr, " ");
		fprintf(stderr, "~ %s\n\n", get_err_str());
		exit(1);
	}
	float a = 1.0;

	int edit_mode = 0;
	int use_aabb = 0;
	for (;;) {
		draw(screen, camera, a, use_aabb);
		if (edit_mode)
			draw_edit(edit, screen, 0x00bebebe, 0);
		SDL_Flip(screen);
		handle_events(screen, &camera, &a, edit, &edit_mode, &use_aabb);
		handle_stats(&thread_data.pixels);
	}
	return 0;
}

