#include "core/model/Texture.h"
#include "core/math/type.h"
#include <SDL/SDL.h>
#include <iostream>
#include <string>

using namespace std;



// TODO methods for texture diffuse (color), ambient (occlusion), normal (need normalize), specular (reflection), shininness (surface noise diffusion)
Texture* loadTextureFromBitmap(string const& filename)
{
	// Load image with SDL
	SDL_Surface* bmp = SDL_LoadBMP(filename.c_str());

	if(bmp == NULL)
		cout << "Error: Cannot open " << filename << endl;

	if(bmp->h != bmp->h || !Texture::isSizeValid(bmp->h))
		cout << "Error: The bitmap image must be square to be used as a "
		        "texture and the size must be a power of 2"
		     << endl;

	const positive size = (positive) bmp->w;
	vec4 data = new real[size * size * VEC4_SCALARS_COUNT];

	for(positive y = 0; y < size; y++) {
		for(positive x = 0; x < size; x++) {
			positive r, g, b;
			getPixel(bmp, x, y, r, g, b);
			positive offset  = (x + y * size) * VEC4_SCALARS_COUNT;
			data[offset + 0] = (real) r / 255;
			data[offset + 1] = (real) g / 255;
			data[offset + 2] = (real) b / 255;
		}
	}

	return new Texture(data, size);
}
