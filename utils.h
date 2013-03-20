
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef UTILS_H
#define UTILS_H
#include "vector.h"
#include "matrix.h"
#include "stdio.h"

void v4sf_print(v4sf a)
{
	float *v = (float *)&a;
	printf("[%g %g %g %g];\n", v[0], v[1], v[2], v[3]);
}

void v4sf_print_name(v4sf a, char *name)
{
	float *v = (float *)&a;
	printf("%s = [%g %g %g %g];\n", name, v[0], v[1], v[2], v[3]);
}

void m4sf_print(m4sf a)
{
	printf("[\n\t");
	v4sf_print(a.v0);
	printf("\t");
	v4sf_print(a.v1);
	printf("\t");
	v4sf_print(a.v2);
	printf("\t");
	v4sf_print(a.v3);
	printf("];\n");
}

void m4sf_print_name(m4sf a, char *name)
{
	printf("%s = ", name);
	m4sf_print(a);
}

#endif

