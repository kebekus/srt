
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "ray.h"

float curve_xyza(float x, float y, float z, float a);
float deriv_x(float x, float y, float z, float a);
float deriv_y(float x, float y, float z, float a);
float deriv_z(float x, float y, float z, float a);

float curve(v4sf v)
{
	return curve_xyza(v[0], v[1], v[2], 0);
}

v4sf gradient(v4sf v)
{
	return v4sf_set3(
		deriv_x(v[0], v[1], v[2], 0),
		deriv_y(v[0], v[1], v[2], 0),
		deriv_z(v[0], v[1], v[2], 0)
	);
}

#define coarse (0.1)
#define fine (0.01)

v4sf value(float l[2], struct ray ray)
{

	float a = curve(ray.o + v4sf_set1(l[0]) * ray.d);
	v4sf p = ray.o + v4sf_set1(l[0]) * ray.d;
	float n = - curve(p) / v4sf_dot3(ray.d, gradient(p));
	while (l[0] < l[1]) {
		float x1 = l[0] + coarse;
		float sign = a * curve(ray.o + v4sf_set1(x1) * ray.d);
		if (sign <= 0) {
			while (l[0] < x1) {
				float x01 = l[0] + fine;
				float sign = a * curve(ray.o + v4sf_set1(x01) * ray.d);
				if (sign <= 0) {
					l[1] = x01;
					goto end;
				}
				l[0] = x01;
			}
			l[1] = x1;
			break;
		}
		v4sf p1 = ray.o + v4sf_set1(x1) * ray.d;
		float n1 = - curve(p1) / v4sf_dot3(ray.d, gradient(p1));
		if ((0 < n && n < 0.1) || (-0.1 < n1 && n1 < 0)) {
			while (l[0] < x1) {
				float x01 = l[0] + fine;
				float sign = a * curve(ray.o + v4sf_set1(x01) * ray.d);
				if (sign <= 0) {
					l[1] = x01;
					goto end;
				}
				l[0] = x01;
			}
		}
		p = p1;
		n = n1;
		l[0] = x1;
	}
end:
	if (l[0] >= l[1])
		return v4sf_set1(0);
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

