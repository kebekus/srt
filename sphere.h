
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef SPHERE_H
#define SPHERE_H
#include "vector.h"
#include "ray.h"

struct sphere
{
	v4sf c;
	float r;
};

static inline int sphere_ray(float l[2], struct sphere sphere, struct ray ray)
{
	v4sf dst = sphere.c - ray.o;
	float b = v4sf_dot3(dst, ray.d);
	float c = v4sf_dot3(dst, dst) - sphere.r * sphere.r;
	float d = b * b - c;
	float q = sqrtf(d);
	l[0] = b - q;
	l[1] = b + q;
	return l[0] < l[1];
}
#endif

