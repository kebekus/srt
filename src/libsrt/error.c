
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "error.h"

static char *error_string = 0;
static int error_position = 0;

char *get_err_str()
{
	return error_string;
}

int get_err_pos()
{
	return error_position;
}

int set_err_str(char *what)
{
	error_string = what;
	return 0;
}

int set_err_pos(int pos)
{
	error_position = pos;
	return 0;
}

int set_err_str_pos(char *what, int pos)
{
	error_string = what;
	error_position = pos;
	return 0;
}

