
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef MATRIX_H
#define MATRIX_H
#include <math.h>
#include <emmintrin.h>
#include "vector.h"

typedef struct {
	v4sf v0;
	v4sf v1;
	v4sf v2;
	v4sf v3;
} m4sf;

typedef struct {
	v4sf x;
	v4sf y;
	v4sf z;
} m34sf;

typedef struct {
	v4si x;
	v4si y;
	v4si z;
} m34si;

typedef struct {
	v4su x;
	v4su y;
	v4su z;
} m34su;

static inline m34sf m34sf_splat(v4sf v)
{
	return (m34sf) {
		v4sf_splat0(v),
		v4sf_splat1(v),
		v4sf_splat2(v)
	};
}

static inline m34sf m34sf_rcp(m34sf m)
{
	return (m34sf) {
		v4sf_set1(1) / m.x,
		v4sf_set1(1) / m.y,
		v4sf_set1(1) / m.z
	};
}

static inline m34su m34sf_ge(m34sf l, v4sf r)
{
	return (m34su) {
		v4sf_ge(l.x, v4sf_splat0(r)),
		v4sf_ge(l.y, v4sf_splat1(r)),
		v4sf_ge(l.z, v4sf_splat2(r))
	};
}

static inline m34sf m34sf_add(m34sf l, m34sf r)
{
	return (m34sf) {
		l.x + r.x,
		l.y + r.y,
		l.z + r.z
	};
}

static inline m34sf m34sf_addv(m34sf l, v4sf r)
{
	return (m34sf) {
		l.x + v4sf_splat0(r),
		l.y + v4sf_splat1(r),
		l.z + v4sf_splat2(r)
	};
}

static inline m34sf m34sf_fma(m34sf a, m34sf b, v4sf c)
{
	return (m34sf) {
		a.x + b.x * c,
		a.y + b.y * c,
		a.z + b.z * c
	};
}

static inline m34sf m34sf_subv(m34sf l, v4sf r)
{
	return (m34sf) {
		l.x - v4sf_splat0(r),
		l.y - v4sf_splat1(r),
		l.z - v4sf_splat2(r)
	};
}

static inline m34sf m34sf_vsub(v4sf l, m34sf r)
{
	return (m34sf) {
		v4sf_splat0(l) - r.x,
		v4sf_splat1(l) - r.y,
		v4sf_splat2(l) - r.z
	};
}

static inline v4sf m34sf_dot(m34sf l, m34sf r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

static inline m34sf m34sf_vmul(m34sf l, v4sf r)
{
	return (m34sf) {
		l.x * r,
		l.y * r,
		l.z * r
	};
}

static inline m34sf m34sf_mul(m34sf l, m34sf r)
{
	return (m34sf) {
		l.x * r.x,
		l.y * r.y,
		l.z * r.z
	};
}

static inline m34sf m34sf_normal(m34sf m)
{
	return m34sf_vmul(m, v4sf_rcp(v4sf_sqrt(m34sf_dot(m, m))));
}

static inline m34sf m34sf_set(v4sf v0, v4sf v1, v4sf v2, v4sf v3)
{
	return (m34sf) {
		{ v0[0], v1[0], v2[0], v3[0] },
		{ v0[1], v1[1], v2[1], v3[1] },
		{ v0[2], v1[2], v2[2], v3[2] }
	};
}

static inline v4sf m34sf_get0(m34sf m)
{
	return (v4sf) { m.x[0], m.y[0], m.z[0], 0 };
}

static inline v4sf m34sf_get1(m34sf m)
{
	return (v4sf) { m.x[1], m.y[1], m.z[1], 0 };
}

static inline v4sf m34sf_get2(m34sf m)
{
	return (v4sf) { m.x[2], m.y[2], m.z[2], 0 };
}

static inline v4sf m34sf_get3(m34sf m)
{
	return (v4sf) { m.x[3], m.y[3], m.z[3], 0 };
}

static inline m4sf m4sf_identity()
{
	return (m4sf) {
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	};
}

static inline m4sf m4sf_mul(m4sf l, m4sf r)
{
	return (m4sf) {
		v4sf_splat0(l.v0) * r.v0 + v4sf_splat1(l.v0) * r.v1 + v4sf_splat2(l.v0) * r.v2 + v4sf_splat3(l.v0) * r.v3,
		v4sf_splat0(l.v1) * r.v0 + v4sf_splat1(l.v1) * r.v1 + v4sf_splat2(l.v1) * r.v2 + v4sf_splat3(l.v1) * r.v3,
		v4sf_splat0(l.v2) * r.v0 + v4sf_splat1(l.v2) * r.v1 + v4sf_splat2(l.v2) * r.v2 + v4sf_splat3(l.v2) * r.v3,
		v4sf_splat0(l.v3) * r.v0 + v4sf_splat1(l.v3) * r.v1 + v4sf_splat2(l.v3) * r.v2 + v4sf_splat3(l.v3) * r.v3
	};
}

static inline v4sf m4sf_vmul(m4sf l, v4sf r)
{
	return (v4sf) {
		v4sf_dot(l.v0, r),
		v4sf_dot(l.v1, r),
		v4sf_dot(l.v2, r),
		v4sf_dot(l.v3, r),
	};
}

static inline m4sf m4sf_rot(v4sf v, float a)
{
	float c = cosf(a);
	float s = sinf(a);
	return (m4sf) {
		{ v[0]*v[0]*(1-c)+c,		v[0]*v[1]*(1-c)-v[2]*s,	v[0]*v[2]*(1-c)+v[1]*s,	0 },
		{ v[1]*v[0]*(1-c)+v[2]*s,	v[1]*v[1]*(1-c)+c,	v[1]*v[2]*(1-c)-v[0]*s,	0 },
		{ v[2]*v[0]*(1-c)-v[1]*s,	v[2]*v[1]*(1-c)+v[0]*s,	v[2]*v[2]*(1-c)+c,	0 },
		{ 0,				0,			0,			1 }
	};
}
#endif

