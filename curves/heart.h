
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "vector.h"

extern inline float curve(v4sf v)
{
	// (2*x^2 + y^2 + z^2 - 1)^3 - 0.1*x^2*z^3 - y^2*z^3
	return cbf(2 * sqf(v[0]) + sqf(v[1]) + sqf(v[2]) - 1) - 0.1 * sqf(v[0]) * cbf(v[2]) - sqf(v[1]) * cbf(v[2]);
}

extern inline v4sf gradient(v4sf v)
{
	// (4*x*(x^2 + y^2 + z^2 + 0.2) - 2*x, 4*y*(x^2 + y^2 + z^2 + 0.2) - 2*y, 4*z*(x^2 + y^2 + z^2 + 0.2))
	return v4sf_set3(
		12 * v[0] * sqf(2 * sqf(v[0]) + sqf(v[1]) + sqf(v[2]) - 1) - 0.2 * v[0] * cbf(v[2]),
		6 * v[1] * sqf(2 * sqf(v[0]) + sqf(v[1]) + sqf(v[2]) - 1) - 2 * v[1] * cbf(v[2]),
		6 * v[2] * sqf(2 * sqf(v[0]) + sqf(v[1]) + sqf(v[2]) - 1) - 3 * sqf(v[1]) * sqf(v[2]) - 0.3 * sqf(v[0]) * sqf(v[2])
	);

}

