
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef SPHERE_H
#define SPHERE_H
#include "ray.h"
#include "ia.h"

struct sphere
{
	v4sf c;
	float r;
};

static inline v4su sphere_ray(i4sf *l, struct sphere sphere, struct ray ray)
{
	m34sf dst = m34sf_vsub(sphere.c, ray.o);
	v4sf b = m34sf_dot(dst, ray.d);
	v4sf c = m34sf_dot(dst, dst) - v4sf_set1(sphere.r * sphere.r);
	v4sf d = b * b - c;
	v4sf q = v4sf_sqrt(d);
	l->min = b - q;
	l->max = b + q;
	return v4sf_lt(l->min, l->max);
}
#endif

