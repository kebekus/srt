
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#define xstr(s) #s
#define str(s) xstr(s)
#include str(CURVE)
#include "ray.h"

v4sf value(float l[2], struct ray ray)
{

	float a = curve(ray.o + v4sf_set1(l[0]) * ray.d);
	float b = a;
	for (float len = l[0] + 0.1; len < l[1]; len += 0.1) {
		b = curve(ray.o + v4sf_set1(len) * ray.d);
		if (a * b <= 0) {
			l[1] = len;
			break;
		}
		l[0] = len;
	}
	if (a * b > 0)
		return v4sf_set1(0);
	v4sf p;
	for (int i = 0; i < 10; i++) {
		float mid = 0.5 * (l[0] + l[1]);
		p = ray.o + v4sf_set1(mid) * ray.d;
		float value = curve(p);
		if (a * value > 0)
			l[0] = mid;
		if (b * value > 0)
			l[1] = mid;
	}
	float tmp = v4sf_dot3(ray.d, v4sf_normal3(gradient(p)));
	if (tmp < 0)
		return v4sf_set3(0, -tmp, 0);
	else
		return v4sf_set3(tmp, 0, 0);
}

