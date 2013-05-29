
/*
edit - line editing in SDL
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdlib.h>
#include <string.h>
#include "edit.h"

int max(int a, int b)
{
	return a > b ? a : b;
}

int min(int a, int b)
{
	return a < b ? a : b;
}

void reset_edit(struct edit *edit, char *str)
{
	strcpy(edit->str, str);
	edit->cursor = strlen(str);
}

struct edit *alloc_edit(int len, char *str)
{
	struct edit *edit = malloc(sizeof(struct edit));
	edit->str = malloc(len);
	strcpy(edit->str, str);
	edit->len = len;
	edit->cursor = strlen(str);
	return edit;
}

void free_edit(struct edit *edit)
{
	free(edit->str);
	free(edit);
}

void draw_char(char chr, uint32_t *fbp, int w, int h, int x0, int y0, uint32_t fg, uint32_t bg, int invert)
{
#include "ascii.h"
	const int font_w = 10;
	const int font_h = 20;
	if (chr < 32 || 126 < chr)
		chr = ' ';
	int c = (chr - 32) * font_w * font_h;
	if (w < (x0 + font_w) || h < (y0 + font_h))
		return;
	for (int j = 0; j < font_h; j++) {
		for (int k = 0; k < font_w; k++) {
			int bit = c + font_w * j + k;
			uint32_t mask = 1 << (bit & 31);
			int index = bit >> 5;
			int x = k + x0;
			int y = j + y0;
			fbp[w * y + x] = !(mask & ascii[index]) != !invert ? fg : bg;
		}
	}
}

void draw_edit(struct edit *edit, SDL_Surface *screen, uint32_t fg, uint32_t bg)
{
	const int font_w = 10;
	const int font_h = 20;
	uint32_t *fbp = (uint32_t *)screen->pixels;
	int w = screen->w;
	int h = min(screen->h, edit->y1);
	int yoff = 0;
	int xoff = 0;
	for (int i = 0; edit->str[i]; i++) {
		draw_char(edit->str[i], fbp, w, h, xoff + edit->x0, yoff + edit->y0, fg, bg, i == edit->cursor);
		xoff += font_w;
		if (edit->x1 < (xoff + edit->x0 + font_w)) {
			yoff += font_h;
			xoff = 0;
		}
	}
	if (!edit->str[edit->cursor])
		draw_char(' ', fbp, w, h, xoff + edit->x0, yoff + edit->y0, fg, bg, 1);
}

int handle_cursor(struct edit *edit, int x, int y)
{
	const int font_w = 10;
	const int font_h = 20;
	int x0 = edit->x0;
	int y0 = edit->y0;
	int x1 = (edit->x1 / font_w) * font_w;
	int y1 = edit->y1;
	if (x < x0 || x1 <= x || y < y0 || y1 <= y)
		return 0;
	int cursor = ((y - y0) / font_h) * ((x1 - x0) / font_w) + (x - x0) / font_w;
	if (cursor < 0 || (int)strlen(edit->str) < cursor)
		return 0;
	edit->cursor = cursor;
	return 1;
}

int handle_chars(SDL_keysym keysym, struct edit *edit)
{
	if (keysym.unicode & 0xFF80)
		return 0;
	char chr = keysym.unicode & 0x7F;
	if (!chr)
		return 0;
	if (('0' <= chr && chr <= '9') || ('a' <= chr && chr <= 'z') || strchr(".+-*/^() ", chr)) {
		memmove(edit->str + edit->cursor + 1, edit->str + edit->cursor, edit->len - edit->cursor - 1);
		edit->str[edit->cursor] = chr;
		edit->cursor++;
		return 1;
	}
	return 0;
}

int handle_edit(SDL_Event event, struct edit *edit)
{
	static int x;
	static int y;
	switch (event.type) {
		case SDL_MOUSEMOTION:
			x = event.motion.x;
			y = event.motion.y;
			return 0;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					return handle_cursor(edit, x, y);
				default:
					return 0;
			}
			return 0;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					edit->cursor -= 0 < edit->cursor ? 1 : 0;
					return 1;
				case SDLK_RIGHT:
					edit->cursor += (int)strlen(edit->str) > edit->cursor ? 1 : 0;
					return 1;
				case SDLK_BACKSPACE:
					if (edit->cursor > 0) {
						memmove(edit->str + edit->cursor - 1, edit->str + edit->cursor, edit->len - edit->cursor);
						edit->cursor--;
					}
					return 1;
				case SDLK_DELETE:
					if (edit->str[edit->cursor])
						memmove(edit->str + edit->cursor, edit->str + edit->cursor + 1, edit->len - edit->cursor);
					return 1;
				default:
					return handle_chars(event.key.keysym, edit);
			}
			return 0;
		default:
			return 0;
	}
	return 0;
}

