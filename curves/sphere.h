
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

extern inline float curve(v4sf v)
{
	// x² + y² + z² - 1
	return v4sf_dot3(v, v) - 1.0;
}

extern inline v4sf gradient(v4sf v)
{
	// (2x, 2y, 2z)
	return v4sf_set1(2) * v;
}

