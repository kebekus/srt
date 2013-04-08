
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
	while (l[0] < l[1]) {
		float x0 = l[0];
		float x1 = l[0] + 0.1;
		float sign = a * curve(ray.o + v4sf_set1(x1) * ray.d);
		if (sign <= 0) {
			l[1] = x1;
			break;
		}
		v4sf p0 = ray.o + v4sf_set1(x0) * ray.d;
		v4sf p1 = ray.o + v4sf_set1(x1) * ray.d;
		float d0 = - curve(p0) / v4sf_dot3(ray.d, gradient(p0));
		float d1 = - curve(p1) / v4sf_dot3(ray.d, gradient(p1));
		if (0 < d0 && d0 < 0.1 && -0.1 < d1 && d1 < 0) {
			while (l[0] < x1) {
				float x01 = l[0] + 0.01;
				float sign = a * curve(ray.o + v4sf_set1(x01) * ray.d);
				if (sign <= 0) {
					l[1] = x01;
					goto end;
				}
				l[0] = x01;
			}
		}
		l[0] = x1;
	}
end:
	if (l[0] >= l[1])
		return v4sf_set1(0);
	float n;
	v4sf p;
	for (int i = 0; i < 10; i++) {
		n = 0.5 * (l[0] + l[1]);
		p = ray.o + v4sf_set1(n) * ray.d;
		float sign = a * curve(p);
		if (sign > 0)
			l[0] = n;
		else
			l[1] = n;
	}
	for (int i = 0; i < 3; i++) {
		n -= curve(p) / v4sf_dot3(ray.d, gradient(p));
		if (n < l[0] || l[1] < n)
			n = 0.5 * (l[0] + l[1]);
		p = ray.o + v4sf_set1(n) * ray.d;
		float sign = a * curve(p);
		if (sign > 0)
			l[0] = n;
		else
			l[1] = n;
	}
	float tmp = v4sf_dot3(ray.d, v4sf_normal3(gradient(p)));
	if (tmp < 0)
		return v4sf_set3(0, -tmp, 0);
	else
		return v4sf_set3(tmp, 0, 0);
}

