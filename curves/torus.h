
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

static inline float curve(v4sf v)
{
	// (x² + y² + z² + 0.2)² - (x² + y²)
	return sqf(v4sf_dot3(v, v) + 0.2) - (sqf(v[0]) + sqf(v[1]));
}

