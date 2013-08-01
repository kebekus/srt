
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"

struct camera
{
	v4sf up, right, dir, origin;
	float near, far;
};

inline struct camera init_camera()
{
	return (struct camera) {
		v4sf_set3(0, 1, 0),
		v4sf_set3(1, 0, 0),
		v4sf_set3(0, 0, -1),
		v4sf_set3(0, 0, 10),
		5, 100
	};
}

#endif
