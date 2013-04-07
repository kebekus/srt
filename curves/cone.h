
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

static inline float curve(v4sf v)
{
	// x² + y² - z²
	return sqf(v[0]) + sqf(v[1]) - sqf(v[2]);
}

static inline v4sf gradient(v4sf v)
{
	// (2x, 2y, -2z)
	return v4sf_set3(2 * v[0], 2 * v[1], -2 * v[2]);
}

