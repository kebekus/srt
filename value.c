
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

v4sf curve(m34sf m, float a)
{
	return curve_xyza(m.x, m.y, m.z, a);
}

m34sf gradient(m34sf m, float a)
{
	return (m34sf) {
		deriv_x(m.x, m.y, m.z, a),
		deriv_y(m.x, m.y, m.z, a),
		deriv_z(m.x, m.y, m.z, a)
	};
}

v4su sign_change(v4sf a, v4sf b)
{
	v4su mask = -v4sf_set1(0);
	return v4si_eq(mask, mask & ((v4su)a ^ (v4su)b));
}

v4sf newton(v4sf n, struct ray ray, float a)
{
	for (int i = 0; i < 20; i++) {
		m34sf p = m34sf_fma(ray.o, ray.d, n);
		n -= curve(p, a) / m34sf_dot(ray.d, gradient(p, a));
	}
	return n;
}

m34sf value(v4sf l[2], struct ray ray, float a)
{
	if (1) {
		m34sf p = m34sf_fma(ray.o, ray.d, l[0]);
		v4sf v = curve(p, a);
		v4su change = v4su_set1(0);
		while (v4su_all_zeros(change | v4sf_ge(l[0], l[1]))) {
			v4sf x = l[0] + (v4sf)(~change & (v4su)v4sf_set1(0.1));
			m34sf p0 = m34sf_fma(ray.o, ray.d, x);
			change |= sign_change(v, curve(p0, a));
			l[0] = v4sf_select(change, l[0], x);
		}
	}
	v4sf n = newton(l[0], ray, a);
	m34sf p = m34sf_fma(ray.o, ray.d, n);
	v4sf v = curve(p, a);
	m34sf p0 = m34sf_fma(ray.o, ray.d, n + v4sf_set1(-0.0001));
	m34sf p1 = m34sf_fma(ray.o, ray.d, n + v4sf_set1(0.0001));
	v4sf v0 = curve(p0, a);
	v4sf v1 = curve(p1, a);
	v4su sign_test = sign_change(v, v0) | sign_change(v, v1);
	v4su int_test = v4sf_le(l[0], n) & v4sf_lt(n, l[1]);
	v4su test = sign_test & int_test;
	v4sf diff = m34sf_dot(ray.d, m34sf_normal(gradient(p, a)));
	v4su face = v4sf_gt(diff, v4sf_set1(0));
	v4sf r = test & face & (v4su)diff;
	v4sf g = test & ~face & (v4su)(-diff);
	v4sf b = v4sf_set1(0);
	return (m34sf) { r, g, b };
}

#if 0
#define coarse (0.1)
#define fine (0.01)

m34sf value(v4sf l[2], struct ray ray, float a)
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
#endif

