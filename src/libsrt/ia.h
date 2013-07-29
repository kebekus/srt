
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef IA_H
#define IA_H
#include "vector.h"

typedef struct {
	v4sf min;
	v4sf max;
} i4sf;

static inline i4sf i4sf_set1(float min, float max)
{
	return (i4sf){{ min, min, min, min }, { max, max, max, max }};
}

static inline i4sf i4sf_set(v4sf min, v4sf max)
{
	return (i4sf){ min, max };
}

static inline i4sf i4sf_mul(i4sf a, i4sf b)
{
	v4sf ab0 = a.min * b.min;
	v4sf ab1 = a.min * b.max;
	v4sf ab2 = a.max * b.min;
	v4sf ab3 = a.max * b.max;
	v4sf min = v4sf_min(v4sf_min(v4sf_min(ab0, ab1), ab2), ab3);
	v4sf max = v4sf_max(v4sf_max(v4sf_max(ab0, ab1), ab2), ab3);
	return (i4sf){ min, max };
}

static inline i4sf i4sf_div(i4sf a, i4sf b)
{
	v4sf ab0 = a.min / b.min;
	v4sf ab1 = a.min / b.max;
	v4sf ab2 = a.max / b.min;
	v4sf ab3 = a.max / b.max;
	v4sf min = v4sf_min(v4sf_min(v4sf_min(ab0, ab1), ab2), ab3);
	v4sf max = v4sf_max(v4sf_max(v4sf_max(ab0, ab1), ab2), ab3);
	return (i4sf){ min, max };
}

static inline i4sf i4sf_add(i4sf a, i4sf b)
{
	return (i4sf){ a.min + b.min, a.max + b.max };
}

static inline i4sf i4sf_sub(i4sf a, i4sf b)
{
	return (i4sf){ a.min - b.max, a.max - b.min };
}

static inline i4sf i4sf_neg(i4sf a)
{
	return (i4sf){ -a.max, -a.min };
}

#endif

