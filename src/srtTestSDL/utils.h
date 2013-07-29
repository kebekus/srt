
/*
sos - SDL OpenGL Skeleton
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef UTILS_H
#define UTILS_H
#include "scalar.h"
#include "vector.h"
#include "matrix.h"
#include "stdio.h"

void v4sf_print(v4sf a)
{
	printf("[%g %g %g %g];\n", a[0], a[1], a[2], a[3]);
}

void v4si_print(v4si a)
{
	printf("[%d %d %d %d];\n", a[0], a[1], a[2], a[3]);
}

void v4sf_print_once(v4sf a)
{
	static int once;
	if (!once) {
		v4sf_print(a);
		once = 1;
	}
}

void v4si_print_once(v4si a)
{
	static int once;
	if (!once) {
		v4si_print(a);
		once = 1;
	}
}

void v4sf_print_name(v4sf a, const char *name)
{
	printf("%s = [%g %g %g %g];\n", name, a[0], a[1], a[2], a[3]);
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

void m4sf_print_name(m4sf a, const char *name)
{
	printf("%s = ", name);
	m4sf_print(a);
}

#endif

