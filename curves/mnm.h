
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

extern inline float curve(v4sf v)
{
	// x^2 + 4 * y^2 + z^2 - 1
	return sqf(v[0]) + 4 * sqf(v[1]) + sqf(v[2]) - 1.0;
}

extern inline v4sf gradient(v4sf v)
{
	// (2*x, 8*y, 2*z)
	return v4sf_set3(2 * v[0], 8 * v[1], 2 * v[2]);
}

