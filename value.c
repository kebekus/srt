
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "stdint.h"
#include "ia.h"
#include "ray.h"
#include "sphere.h"
#include "aabb.h"
#include "camera.h"
#include "stripe_data.h"

#if USE_LLVM
v4sf curve_xyza(v4sf x, v4sf y, v4sf z, float a);
v4sf deriv_x(v4sf x, v4sf y, v4sf z, float a);
v4sf deriv_y(v4sf x, v4sf y, v4sf z, float a);
v4sf deriv_z(v4sf x, v4sf y, v4sf z, float a);
#else
#include "module.h"
#endif

extern inline v4sf curve(m34sf m, float a)
{
	return curve_xyza(m.x, m.y, m.z, a);
}

extern inline m34sf gradient(m34sf m, float a)
{
	return (m34sf) {
		deriv_x(m.x, m.y, m.z, a),
		deriv_y(m.x, m.y, m.z, a),
		deriv_z(m.x, m.y, m.z, a)
	};
}

static inline v4su sign_change(v4sf a, v4sf b)
{
	v4si mask = v4si_set1(0x80000000);
	return v4si_eq(mask, mask & ((v4su)a ^ (v4su)b));
}

static inline v4sf bisect(i4sf l, struct ray ray, float a)
{
	i4sf k = l;
	m34sf p0 = ray_point(k.min, ray);
	v4sf v0 = curve(p0, a);
	for (int i = 0; i < 20; i++) {
		v4sf x = v4sf_set1(0.5) * (k.min + k.max);
		m34sf p1 = ray_point(x, ray);
		v4su test = sign_change(v0, curve(p1, a));
		k.min = v4sf_select(test, k.min, x);
		k.max = v4sf_select(test, x, k.max);
	}
	return v4sf_set1(0.5) * (k.min + k.max);
}

static inline v4sf newton(v4sf n, struct ray ray, float a)
{
	for (int i = 0; i < 20; i++) {
		m34sf p = ray_point(n, ray);
		n -= curve(p, a) / m34sf_dot(ray.d, gradient(p, a));
	}
	return n;
}

static inline v4sf newton_forward(v4sf n, struct ray ray, float a)
{
	for (int i = 0; i < 20; i++) {
		m34sf p = ray_point(n, ray);
		v4sf x = - curve(p, a) / m34sf_dot(ray.d, gradient(p, a));
		n += v4sf_max(x, v4sf_set1(0));
	}
	return n;
}

static inline v4sf newton_bisect(i4sf l, struct ray ray, float a)
{
	i4sf k = l;
	m34sf p0 = ray_point(k.min, ray);
	v4sf v0 = curve(p0, a);
	for (int i = 0; i < 20; i++) {
		v4sf x = v4sf_set1(0.5) * (k.min + k.max);
		m34sf p1 = ray_point(x, ray);
		v4sf n = x - curve(p1, a) / m34sf_dot(ray.d, gradient(p1, a));
		v4su inside = v4sf_lt(k.min, n) & v4sf_lt(n, k.max);
		v4sf nx = v4sf_select(inside, n, x);
		m34sf p2 = ray_point(nx, ray);
		v4su test = sign_change(v0, curve(p2, a));
		k.min = v4sf_select(test, k.min, nx);
		k.max = v4sf_select(test, nx, k.max);
	}
	return v4sf_set1(0.5) * (k.min + k.max);
}

static inline v4su sign_test(v4sf n, struct ray ray, float a)
{
	m34sf p = ray_point(n, ray);
	v4sf v = curve(p, a);
	m34sf p0 = ray_point(n + v4sf_set1(-0.0001), ray);
	m34sf p1 = ray_point(n + v4sf_set1(0.0001), ray);
	v4sf v0 = curve(p0, a);
	v4sf v1 = curve(p1, a);
	return sign_change(v, v0) | sign_change(v, v1);
}

static inline v4su epsilon_test(v4sf n, struct ray ray, float a)
{
	float epsilon = 0.0000000001;
	m34sf p = ray_point(n, ray);
	v4sf v = curve(p, a);
	return v4sf_lt(v4sf_abs(v), v4sf_set1(epsilon));
}

static inline v4su zero_test(v4sf n, struct ray ray, float a)
{
	m34sf p = ray_point(n, ray);
	v4sf v = curve(p, a);
	return v4sf_eq(v4sf_set1(0), v);
}

static inline v4su int_test(v4sf n, i4sf l)
{
	return v4sf_lt(l.min, l.max) & v4sf_le(l.min, n) & v4sf_lt(n, l.max);
}

static inline m34sf color(v4sf n, v4su test, struct ray ray, float a)
{
	m34sf p = ray_point(n, ray);
	v4sf diff = m34sf_dot(ray.d, m34sf_normal(gradient(p, a)));
	v4su face = v4sf_gt(diff, v4sf_set1(0));
	v4sf r = v4sf_and(test & face, diff);
	v4sf g = v4sf_and(test & ~face, -diff);
	v4sf b = v4sf_set1(0);
	return (m34sf) { r, g, b };
}

static inline v4su localize(i4sf *l, struct ray ray, float a)
{
	float coarse = 0.05;
	float fine = 0.001;
	i4sf k = *l;
	m34sf p0 = ray_point(k.min, ray);
	v4sf v0 = curve(p0, a);
	v4su test = v4su_set1(0);
	while (!v4su_all_ones(test | v4sf_ge(k.min, l->max))) {
		v4sf x = v0 / m34sf_dot(ray.d, gradient(p0, a));
		v4sf step = v4sf_clamp(v4sf_abs(x), v4sf_set1(fine), v4sf_set1(coarse));
		k.max = v4sf_select(test, k.max, k.min + step);
		m34sf p1 = ray_point(k.max, ray);
		v4sf v1 = curve(p1, a);
		test |= v4sf_lt(k.max, l->max) & sign_change(v0, v1);
		k.min = v4sf_select(test, k.min, k.max);
		v0 = v1;
		p0 = p1;
	}
	l->min = v4sf_select(test, k.min, l->min);
	l->max = v4sf_select(test, k.max, l->max);
	return test;
}

static inline m34sf value(i4sf l, struct ray ray, float a)
{
	v4su test = localize(&l, ray, a);
//	v4sf n = l.min;
//	v4sf n = bisect(l, ray, a);
//	v4sf n = newton(l.min, ray, a);
	v4sf n = newton(bisect(l, ray, a), ray, a);
//	v4sf n = newton_bisect(l, ray, a);
//	v4sf n = newton_forward(l.min, ray, a);
//	return color(n, sign_test(n, ray, a) & int_test(n, l), ray, a);
//	return color(n, zero_test(n, ray, a) & int_test(n, l), ray, a);
//	return color(n, (sign_test(n, ray, a) | zero_test(n, ray, a)) & int_test(n, l), ray, a);
//	return color(n, (sign_test(n, ray, a) | epsilon_test(n, ray, a)) & int_test(n, l), ray, a);
	return color(n, test, ray, a);
}

static inline uint32_t argb(v4sf c)
{
	v4si rgb = v4sf_cvt(v4sf_clamp(v4sf_set1(255) * c, v4sf_set1(0), v4sf_set1(255)));
	return (rgb[0] << 16) | (rgb[1] << 8) | (rgb[2] << 0);
}

int64_t stripe(struct stripe_data *sd, int j)
{
	uint32_t *fb = sd->fb;
	int w = sd->w;
	v4sf dU = sd->dU;
	v4sf dV = sd->dV;
	m34sf UV = sd->UV;
	struct sphere sphere = sd->sphere;
	struct aabb aabb = sd->aabb;
	struct camera camera = sd->camera;
	float a = sd->a;
	int use_aabb = sd->use_aabb;
	v4sf jdV = v4sf_set1(j) * dV;
	int64_t pixels = 0;
	for (int i = 0; i < w; i += 2) {
		v4sf idU = v4sf_set1(i) * dU;
		m34sf scr = m34sf_addv(UV, jdV + idU);
		m34sf dir = m34sf_normal(m34sf_addv(scr, camera.dir));
		struct ray ray = init_ray(m34sf_splat(camera.origin), dir);
		i4sf l;
		uint32_t color[4] = { 0, 0, 0, 0 };
		v4su test;
		if (use_aabb)
			test = aabb_ray(&l, aabb, ray);
		else
			test = sphere_ray(&l, sphere, ray);
		test &= v4sf_gt(l.max, v4sf_set1(0));
		if (!v4su_all_zeros(test)) {
			pixels += 4;
			l.min = v4sf_max(l.min, v4sf_set1(0));
			l.min = v4sf_and(test, l.min);
			l.max = v4sf_and(test, l.max);
			m34sf tmp = value(l, ray, a);
			color[0] = test[0] & argb(m34sf_get0(tmp));
			color[1] = test[1] & argb(m34sf_get1(tmp));
			color[2] = test[2] & argb(m34sf_get2(tmp));
			color[3] = test[3] & argb(m34sf_get3(tmp));
		}
		fb[w * (j+0) + (i+0)] = color[0];
		fb[w * (j+0) + (i+1)] = color[1];
		fb[w * (j+1) + (i+0)] = color[2];
		fb[w * (j+1) + (i+1)] = color[3];
	}
	return pixels;
}

