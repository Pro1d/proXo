#include "core/model/Texture.h"
#include "core/math/type.h"
#include <SDL/SDL.h>
#include <iostream>
#include <string>

using namespace std;
using namespace proxo;

void getPixel(
    SDL_Surface* surface, positive x, positive y, Uint8& r, Uint8& g, Uint8& b)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*) surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			SDL_GetRGB(*p, surface->format, &r, &g, &b);
			return;

		case 2:
			SDL_GetRGB(*(Uint16*) p, surface->format, &r, &g, &b);
			return;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				r = p[0];
				g = p[1];
				b = p[2];
			}
			else {
				b = p[0];
				g = p[1];
				r = p[2];
			}
			return;

		case 4:
			SDL_GetRGB(*(Uint32*) p, surface->format, &r, &g, &b);
			return;

		default: /* shouldn't happen, but avoids warnings */
			r = 0;
			g = 0;
			b = 0;
	}
}

// TODO methods for texture diffuse (color), ambient (occlusion), normal (need
// normalize), specular (reflection), shininness (surface noise diffusion)
Texture* loadTextureFromBitmap(string const& filename)
{
	// Load image with SDL
	SDL_Surface* bmp = SDL_LoadBMP(filename.c_str());

	if(bmp == NULL)
		cout << "Error: Cannot open " << filename << endl;

	if(bmp->h != bmp->w || !Texture::isSizeValid(bmp->h))
		cout << "Error: The bitmap image must be square to be used as a "
		        "texture and the size must be a power of 2"
		     << endl;

	const positive size = (positive) bmp->w;
	vec4 data           = new real[size * size * VEC4_SCALARS_COUNT];

	for(positive y = 0; y < size; y++) {
		for(positive x = 0; x < size; x++) {
			Uint8 r, g, b;
			getPixel(bmp, x, y, r, g, b);
			positive offset  = (x + y * size) * VEC4_SCALARS_COUNT;
			data[offset + 0] = (real) r / 255;
			data[offset + 1] = (real) g / 255;
			data[offset + 2] = (real) b / 255;
		}
	}

	return new Texture(data, size);
}
