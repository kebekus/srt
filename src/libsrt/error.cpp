
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdlib.h>
#include "error.h"

namespace parser
{
	namespace error
	{
		static int number = 0;
		static int position = 0;

		static const char *strings[] = {
			"ok",
			"argument does not reduce to number",
			"argument is negative",
			"closing parenthese missing",
			"divisor does not reduce to number",
			"divisor is zero",
			"empty expression",
			"exponent does not reduce to number",
			"exponent is negative",
			"exponent not integer",
			"incomplete expression",
			"not enough nodes",
			"number to long",
			"opening parenthese missing",
			"stack overflow",
			"syntax error",
			"there can be only one",
			"unknown",
			"unknown char",
			"unknown token"
		};

		int get_num()
		{
			return number;
		}

		const char *get_str()
		{
			return strings[number];
		}

		int get_pos()
		{
			return position;
		}

		int set_num(int num)
		{
			if (num < 0 || num >= sizeof(strings) / sizeof(*strings))
				abort();
			number = num;
			return 0;
		}

		int set_pos(int pos)
		{
			position = pos;
			return 0;
		}

		int set_num_pos(int num, int pos)
		{
			set_num(num);
			position = pos;
			return 0;
		}
	}
}

