
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef AABB_H
#define AABB_H
#include "ray.h"
#include "ia.h"

struct aabb
{
	v4sf c0, c1;
};

static inline v4su aabb_ray(i4sf *l, struct aabb box, struct ray ray)
{
	m34sf a = m34sf_mul(m34sf_vsub(box.c0, ray.o), ray.inv_d);
	m34sf b = m34sf_mul(m34sf_vsub(box.c1, ray.o), ray.inv_d);
	l->min = v4sf_max(v4sf_max(v4sf_min(a.x, b.x), v4sf_min(a.y, b.y)), v4sf_min(a.z, b.z));
	l->max = v4sf_min(v4sf_min(v4sf_max(a.x, b.x), v4sf_max(a.y, b.y)), v4sf_max(a.z, b.z));
	return v4sf_lt(l->min, l->max);
}
#endif
