
/*
srt - SIMD Ray Tracing
Written in 2013 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "stdio.h"
#include "ppm.h"

int ppm_save(char *name, SDL_Surface *surface)
{
	if (!surface)
		return 0;
	if (surface->format->BytesPerPixel != 4)
		return 0;
	if (surface->pitch != surface->w * 4)
		return 0;
	FILE *file = fopen(name, "w");
	if (!file)
		return 0;
	if (0 >= fprintf(file, "P6 %d %d 255\n", surface->w, surface->h)) {
		fclose(file);
		return 0;
	}
	int num = surface->w * surface->h;
	uint32_t *fb = surface->pixels;
	int err = 0;
	while (num-- &&
		0 <= (err = fputc(255 & (*fb >> 16), file)) &&
		0 <= (err = fputc(255 & (*fb >> 8), file)) &&
		0 <= (err = fputc(255 & (*fb >> 0), file))
	) fb++;

	return !fclose(file) && err >= 0;
}

