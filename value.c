
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

float curve_sisd(v4sf v, float A)
{
	return curve(m34sf_set(v, v, v, v), A)[0];
}

v4sf gradient_sisd(v4sf v, float A)
{
	return m34sf_get0(gradient(m34sf_set(v, v, v, v), A));
}

#define coarse (0.1)
#define fine (0.01)

v4sf value_sisd(float l[2], v4sf ray_d, v4sf ray_o, float A)
{

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

