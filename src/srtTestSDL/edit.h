
/*
edit - line editing in SDL
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/
#ifndef EDIT_H
#define EDIT_H
#include <SDL.h>

struct edit {
	char *str;
	char msg_str[64];
	int msg_pos;
	int len;
	int cursor;
	int rows, cols;
	int x0, y0;
	int x1, y1;
};

struct edit *alloc_edit(int len, char *str);
void reset_edit(struct edit *edit, char *str);
void free_edit(struct edit *edit);
void resize_edit(struct edit *edit, int x0, int y0, int x1, int y1);
void draw_edit(struct edit *edit, SDL_Surface *screen, uint32_t fg, uint32_t bg);
int handle_edit(SDL_Event event, struct edit *edit);
void edit_msg(struct edit *edit, char *msg, int pos);
#endif
