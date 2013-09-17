
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef STRIPE_DATA_H
#define STRIPE_DATA_H

#include <stdint.h>
#include "sphere.h"
#include "aabb.h"
#include "camera.h"

struct stripe_data {
	uint32_t *fb;
	int w;
	int h;
	v4sf dU;
	v4sf dV;
	m34sf UV;
	struct sphere sphere;
	struct aabb aabb;
	struct camera camera;
	float a;
	int use_aabb;
};

#endif

