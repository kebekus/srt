
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "ray.h"

v4sf curve_xyza(v4sf x, v4sf y, v4sf z, float a);
v4sf deriv_x(v4sf x, v4sf y, v4sf z, float a);
v4sf deriv_y(v4sf x, v4sf y, v4sf z, float a);
v4sf deriv_z(v4sf x, v4sf y, v4sf z, float a);

v4sf curve(m34sf m, float A)
{
	return curve_xyza(m.x, m.y, m.z, A);
}

m34sf gradient(m34sf m, float A)
{
	return (m34sf) {
		deriv_x(m.x, m.y, m.z, A),
		deriv_y(m.x, m.y, m.z, A),
		deriv_z(m.x, m.y, m.z, A)
	};
}

#define coarse (0.1)
#define fine (0.01)

m34sf value(v4sf l[2], struct ray ray, float A)
{
	m34sf p = m34sf_add(ray.o, m34sf_vmul(ray.d, l[0]));
	v4sf n = - curve(p, A) / m34sf_dot(ray.d, gradient(p, A));
	for (int i = 0; i < 20; i++) {
		n -= curve(p, A) / m34sf_dot(ray.d, gradient(p, A));
		p = m34sf_add(ray.o, m34sf_vmul(ray.d, n));
	}
	v4sf diff = m34sf_dot(ray.d, m34sf_normal(gradient(p, A)));
	v4su face = v4sf_gt(diff, v4sf_set1(0));

	m34sf p0 = m34sf_add(ray.o, m34sf_vmul(ray.d, n + v4sf_set1(-fine)));
	m34sf p1 = m34sf_add(ray.o, m34sf_vmul(ray.d, n + v4sf_set1(fine)));
	v4su sign_test = v4sf_gt(v4sf_set1(0), curve(p0, A) * curve(p1, A));
	v4su int_test = v4sf_ge(n, l[0]) & v4sf_gt(l[1], n);
	v4su test = sign_test & int_test;

	m34sf color = { test & face & (v4su)diff, test & (~face) & (v4su)(-diff), v4sf_set1(0) };
	return color;
//	return m34sf_set(v4sf_set1(0), v4sf_set1(0), v4sf_set1(0), v4sf_set1(0));
#if 0
	float a = curve_sisd(ray_o + v4sf_set1(l[0]) * ray_d, A);
	v4sf p = ray_o + v4sf_set1(l[0]) * ray_d;
	float n = - curve_sisd(p, A) / v4sf_dot3(ray_d, gradient_sisd(p, A));
	while (l[0] < l[1]) {
		float x1 = l[0] + coarse;
		float sign = a * curve_sisd(ray_o + v4sf_set1(x1) * ray_d, A);
		if (sign <= 0) {
			while (l[0] < x1) {
				float x01 = l[0] + fine;
				float sign = a * curve_sisd(ray_o + v4sf_set1(x01) * ray_d, A);
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
		float n1 = - curve_sisd(p1, A) / v4sf_dot3(ray_d, gradient_sisd(p1, A));
		if ((0 < n && n < 0.1) || (-0.1 < n1 && n1 < 0)) {
			while (l[0] < x1) {
				float x01 = l[0] + fine;
				float sign = a * curve_sisd(ray_o + v4sf_set1(x01) * ray_d, A);
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
		float sign = a * curve_sisd(p, A);
		if (sign > 0)
			l[0] = n;
		else
			l[1] = n;
	}
	for (int i = 0; i < 3; i++) {
		n -= curve_sisd(p, A) / v4sf_dot3(ray_d, gradient_sisd(p, A));
		if (n < l[0] || l[1] < n)
			n = 0.5 * (l[0] + l[1]);
		p = ray_o + v4sf_set1(n) * ray_d;
		float sign = a * curve_sisd(p, A);
		if (sign > 0)
			l[0] = n;
		else
			l[1] = n;
	}
	float tmp = v4sf_dot3(ray_d, v4sf_normal3(gradient_sisd(p, A)));
	if (tmp < 0)
		return v4sf_set3(0, -tmp, 0);
	else
		return v4sf_set3(tmp, 0, 0);
#endif
}

