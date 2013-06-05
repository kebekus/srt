
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

float curve(v4sf v, float A)
{
	return curve_xyza(v[0], v[1], v[2], A);
}

v4sf gradient(v4sf v, float A)
{
	return v4sf_set3(
		deriv_x(v[0], v[1], v[2], A),
		deriv_y(v[0], v[1], v[2], A),
		deriv_z(v[0], v[1], v[2], A)
	);
}

#define coarse (0.1)
#define fine (0.01)

v4sf value_sisd(float l[2], v4sf ray_d, v4sf ray_o, float A)
{

	float a = curve(ray_o + v4sf_set1(l[0]) * ray_d, A);
	v4sf p = ray_o + v4sf_set1(l[0]) * ray_d;
	float n = - curve(p, A) / v4sf_dot3(ray_d, gradient(p, A));
	while (l[0] < l[1]) {
		float x1 = l[0] + coarse;
		float sign = a * curve(ray_o + v4sf_set1(x1) * ray_d, A);
		if (sign <= 0) {
			while (l[0] < x1) {
				float x01 = l[0] + fine;
				float sign = a * curve(ray_o + v4sf_set1(x01) * ray_d, A);
				if (sign <= 0) {
					l[1] = x01;
					goto end;
				}
				l[0] = x01;
			}
			l[1] = x1;
			break;
		}
		v4sf p1 = ray_o + v4sf_set1(x1) * ray_d;
		float n1 = - curve(p1, A) / v4sf_dot3(ray_d, gradient(p1, A));
		if ((0 < n && n < 0.1) || (-0.1 < n1 && n1 < 0)) {
			while (l[0] < x1) {
				float x01 = l[0] + fine;
				float sign = a * curve(ray_o + v4sf_set1(x01) * ray_d, A);
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
		p = ray_o + v4sf_set1(n) * ray_d;
		float sign = a * curve(p, A);
		if (sign > 0)
			l[0] = n;
		else
			l[1] = n;
	}
	for (int i = 0; i < 3; i++) {
		n -= curve(p, A) / v4sf_dot3(ray_d, gradient(p, A));
		if (n < l[0] || l[1] < n)
			n = 0.5 * (l[0] + l[1]);
		p = ray_o + v4sf_set1(n) * ray_d;
		float sign = a * curve(p, A);
		if (sign > 0)
			l[0] = n;
		else
			l[1] = n;
	}
	float tmp = v4sf_dot3(ray_d, v4sf_normal3(gradient(p, A)));
	if (tmp < 0)
		return v4sf_set3(0, -tmp, 0);
	else
		return v4sf_set3(tmp, 0, 0);
}

m34sf value(v4sf l[2], struct ray ray, float A)
{
	float l0[2] = { l[0][0], l[1][0] };
	float l1[2] = { l[0][1], l[1][1] };
	float l2[2] = { l[0][2], l[1][2] };
	float l3[2] = { l[0][3], l[1][3] };
	v4sf ray_d0 = m34sf_get0(ray.d);
	v4sf ray_d1 = m34sf_get1(ray.d);
	v4sf ray_d2 = m34sf_get2(ray.d);
	v4sf ray_d3 = m34sf_get3(ray.d);
	v4sf ray_o0 = m34sf_get0(ray.o);
	v4sf ray_o1 = m34sf_get1(ray.o);
	v4sf ray_o2 = m34sf_get2(ray.o);
	v4sf ray_o3 = m34sf_get3(ray.o);
	v4sf v0 = value_sisd(l0, ray_d0, ray_o0, A);
	v4sf v1 = value_sisd(l1, ray_d1, ray_o1, A);
	v4sf v2 = value_sisd(l2, ray_d2, ray_o2, A);
	v4sf v3 = value_sisd(l3, ray_d3, ray_o3, A);
	l[0] = v4sf_set(l0[0], l1[0], l2[0], l3[0]);
	l[1] = v4sf_set(l0[1], l1[1], l2[1], l3[1]);
	return m34sf_set(v0, v1, v2, v3);
}

