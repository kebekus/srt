
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

static inline float curve(v4sf v)
{
	// z
	return v[2];
}

static inline v4sf gradient(v4sf v)
{
	// (0, 0, 1)
	(void)v;
	return v4sf_set3(0, 0, 1);
}

