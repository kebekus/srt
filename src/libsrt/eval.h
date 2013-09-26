
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef EVAL_H
#define EVAL_H
namespace parser
{
	float func_pow(float left, float right);
	float func_mul(float left, float right);
	float func_div(float left, float right);
	float func_add(float left, float right);
	float func_sub(float left, float right);
	float func_neg(float left, float right);
	float func_sqrt(float left, float right);
	float func_sin(float left, float right);
	float func_cos(float left, float right);
	float eval_branch(struct node *node, float x, float y, float z, float a);
	float eval(struct tree *tree, float x, float y, float z, float a);
}
#endif
