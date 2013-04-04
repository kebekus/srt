
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

static inline float curve(v4sf v)
{
	// (2x² + y² + z² - 1)³ - x²z³/10 - y²z³
	return cbf(2 * sqf(v[0]) + sqf(v[1]) + sqf(v[2]) - 1) - 0.1 * sqf(v[0]) * cbf(v[2]) - sqf(v[1]) * cbf(v[2]);
}
