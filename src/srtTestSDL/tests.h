
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef TESTS_H
#define TESTS_H

#include <stdlib.h>
#include "vector.h"
#include "matrix.h"
#include "utils.h"

void matrix_tests()
{
	v4sf V = { 1, 2, 3, 4 };
	v4sf_print_name(V, "V");
	m4sf A = {
		{ 1, 2, 3, 4 },
		{ 5, 6, 7, 8 },
		{ 9, 10, 11, 12 },
		{ 13, 14, 15, 16 }
	};
	m4sf_print_name(A, "A");
	m4sf B = {
		{ 17, 18, 19, 20 },
		{ 21, 22, 23, 24 },
		{ 25, 26, 27, 28 },
		{ 29, 30, 31, 32 }
	};
	m4sf_print_name(B, "B");
	m4sf_print_name(m4sf_mul(A, B), "A * B");
}
#endif
