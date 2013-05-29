
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
	int len;
	int cursor;
	int x0, y0;
	int x1, y1;
};

struct edit *alloc_edit(int len, char *str);
void reset_edit(struct edit *edit, char *str);
void free_edit(struct edit *edit);
void draw_edit(struct edit *edit, SDL_Surface *screen, uint32_t fg, uint32_t bg);
int handle_edit(SDL_Event event, struct edit *edit);
#endif
