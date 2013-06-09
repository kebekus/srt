
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef RAY_H
#define RAY_H
#include "matrix.h"

struct ray
{
	m34sf o, d;
	m34sf inv_d;
	m34su sign;
};

static inline m34sf ray_point(v4sf l, struct ray ray)
{
	return m34sf_fma(ray.o, ray.d, l);
}

static inline struct ray init_ray(m34sf o, m34sf d)
{
	struct ray ray;
	ray.o = o;
	ray.d = d;
	ray.inv_d = m34sf_rcp(d);
	ray.sign = m34sf_ge(d, v4sf_set1(0));
	return ray;
}

#endif
