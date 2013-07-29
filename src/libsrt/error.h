
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef ERROR_H
#define ERROR_H

char *get_err_str();
int get_err_pos();
int set_err_str(char *what);
int set_err_pos(int pos);
int set_err_str_pos(char *what, int pos);

#endif
