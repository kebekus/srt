
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef SCALAR_H
#define SCALAR_H
#include <math.h>

static inline float sqf(float f)
{
	return f * f;
}

static inline float cbf(float f)
{
	return f * f * f;
}

#endif

