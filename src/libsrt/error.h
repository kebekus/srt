
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef ERROR_H
#define ERROR_H

namespace parser
{
	namespace error
	{
		enum {
			ok = 0,
			argument_does_not_reduce_to_number,
			argument_is_negative,
			closing_parenthese_missing,
			divisor_does_not_reduce_to_number,
			divisor_is_zero,
			empty_expression,
			exponent_does_not_reduce_to_number,
			exponent_is_negative,
			exponent_not_integer,
			incomplete_expression,
			not_enough_nodes,
			number_to_long,
			opening_parenthese_missing,
			stack_overflow,
			syntax_error,
			there_can_be_only_one,
			unknown,
			unknown_char,
			unknown_token
		};
		int get_num();
		const char *get_str();
		int get_pos();
		int set_num(int num);
		int set_pos(int pos);
		int set_num_pos(int num, int pos);
	}

}

#endif
