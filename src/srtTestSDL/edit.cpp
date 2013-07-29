
/*
edit - line editing in SDL
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdlib.h>
#include <string.h>
#include "edit.h"
#include "ascii.h"
const int font_w = 10;
const int font_h = 20;

void reset_edit(struct edit *edit, const char *str)
{
	strcpy(edit->str, str);
	edit->cursor = strlen(str);
}

void edit_msg(struct edit *edit, const char *msg, int pos)
{
	if (!msg || !*msg) {
		edit->msg_str[0] = 0;
		edit->msg_pos = 0;
		return;
	}
	strcpy(edit->msg_str, msg);
	edit->msg_pos = pos;
}

struct edit *alloc_edit(int len, const char *str)
{
	struct edit *edit = (struct edit*)malloc(sizeof(struct edit));
	edit->str = (char *)malloc(len);
	strcpy(edit->str, str);
	edit->len = len;
	edit->cursor = strlen(str);
	edit->msg_str[0] = 0;
	edit->msg_pos = 0;
	return edit;
}

void free_edit(struct edit *edit)
{
	free(edit->str);
	free(edit);
}

void resize_edit(struct edit *edit, int x0, int y0, int x1, int y1)
{
	edit->x0 = x0;
	edit->y0 = y0;
	edit->x1 = x1;
	edit->y1 = y1;
	edit->cols = (x1 - x0) / font_w;
	edit->rows = (y1 - y0) / font_h;
}

void draw_char(struct edit *edit, SDL_Surface *screen, uint32_t fg, uint32_t bg, int row, int col, char chr, int invert)
{
	uint32_t *fbp = (uint32_t*)screen->pixels;
	int w = screen->w;
	if (row < 0 || row >= edit->rows || col < 0 || col >= edit->cols)
		return;
	if (chr < 32 || 126 < chr)
		chr = ' ';
	int c = (chr - 32) * font_w * font_h;
	for (int j = 0; j < font_h; j++) {
		for (int k = 0; k < font_w; k++) {
			int bit = c + font_w * j + k;
			uint32_t mask = 1 << (bit & 31);
			int index = bit >> 5;
			int x = k + edit->x0 + font_w * col;
			int y = j + edit->y0 + font_h * row;
			fbp[w * y + x] = !(mask & ascii[index]) != !invert ? fg : bg;
		}
	}
}

int draw_msg(struct edit *edit, SDL_Surface *screen, uint32_t fg, uint32_t bg, int row, int col)
{
	if (!edit->msg_str[0])
		return 0;
	draw_char(edit, screen, fg, bg, row + 1, col, '~', 0);
	int offset = 2;
	int msg_len = strlen(edit->msg_str) + 2;
	if ((msg_len + col) > edit->cols && (msg_len <= col || (2 * col) >= edit->cols))
		offset = - (strlen(edit->msg_str) + 1);
	for (int m = 0; edit->msg_str[m]; m++)
		draw_char(edit, screen, fg, bg, row + 1, col + offset + m, edit->msg_str[m], 0);
	return 1;
}

void draw_edit(struct edit *edit, SDL_Surface *screen, uint32_t fg, uint32_t bg)
{
	int row = 0;
	int col = 0;
	int msg = 0;
	int len = strlen(edit->str);
	for (int i = 0; i <= len; i++) {
		if (i == edit->msg_pos && draw_msg(edit, screen, fg, bg, row, col))
			msg = 1;
		draw_char(edit, screen, fg, bg, row, col++, edit->str[i], i == edit->cursor);
		if (col >= edit->cols) {
			row++;
			row += msg;
			msg = 0;
			col = 0;
		}
	}
}

int handle_cursor(struct edit *edit, int x, int y)
{
	int x0 = edit->x0;
	int y0 = edit->y0;
	int x1 = edit->x0 + edit->cols * font_w;
	int y1 = edit->y0 + edit->rows * font_h;
	if (x < x0 || x1 <= x || y < y0 || y1 <= y)
		return 0;
	int row = (y - y0) / font_h;
	int col = (x - x0) / font_w;
	if (edit->msg_str[0]) {
		int msg_row = edit->msg_pos / edit->cols + 1;
		if (row == msg_row)
			return 0;
		if (row > msg_row)
			row--;
	}
	int cursor = row * edit->cols + col;
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
	if (('0' <= chr && chr <= '9') || ('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z') || strchr(".+-*/^() ", chr)) {
		memmove(edit->str + edit->cursor + 1, edit->str + edit->cursor, edit->len - edit->cursor - 1);
		edit->str[edit->cursor] = chr;
		edit->cursor++;
		edit_msg(edit, 0, 0);
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
				case SDLK_HOME:
					edit->cursor = 0;
					return 1;
				case SDLK_END:
					edit->cursor = strlen(edit->str);
					return 1;
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
						edit_msg(edit, 0, 0);
					}
					return 1;
				case SDLK_DELETE:
					if (edit->str[edit->cursor]) {
						memmove(edit->str + edit->cursor, edit->str + edit->cursor + 1, edit->len - edit->cursor);
						edit_msg(edit, 0, 0);
					}
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

