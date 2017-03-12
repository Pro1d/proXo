#include "TextureLoader.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include "core/model/Texture.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
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

TextureLoader::TextureLoader() : size_(0), flagFields_(0)
{
	for(positive i = 0; i < Texture::FIELDS_COUNT; i++)
		bitmaps_[i] = NULL;
}

TextureLoader::~TextureLoader()
{
	for(positive i = 0; i < Texture::FIELDS_COUNT; i++) {
		if(bitmaps_[i] != NULL) {
			SDL_FreeSurface(bitmaps_[i]);
			bitmaps_[i] = NULL;
		}
	}
}

void TextureLoader::reset()
{
	size_ = 0;
	flagFields_ = 0;
	for(positive i = 0; i < Texture::FIELDS_COUNT; i++) {
		if(bitmaps_[i] != NULL) {
			SDL_FreeSurface(bitmaps_[i]);
			bitmaps_[i] = NULL;
		}
	}
}

void TextureLoader::addImageFile(const string& filename, Texture::Field field)
{
	SDL_Surface * bmp = loadBitmap(filename);
	if(bmp != NULL) {
		bitmaps_[field] = bmp;
		flagFields_ |= ((positive) 1) << field;
	}
}

SDL_Surface* TextureLoader::loadBitmap(const string& filename)
{
	SDL_Surface* bmp = IMG_Load(filename.c_str());

	if(bmp == NULL) {
		cout << "Error: Cannot open " << filename << endl;
		return NULL;
	}

	if(bmp->h != bmp->w || !Texture::isSizeValid(bmp->h)) {
		cout << "Error: The image must be square to be used as a "
		        "texture and the size must be a power of 2"
		     << endl;
		SDL_FreeSurface(bmp);
		return NULL;
	}

	if(size_ > 0 && size_ != (positive) bmp->w) {
		cout << "Error: The images in the same texture must have the same size."
		     << endl;
		SDL_FreeSurface(bmp);
		return NULL;
	}

	if(size_ == 0)
		size_ = bmp->w;

	return bmp;
}

Texture* TextureLoader::createTexture()
{
	Texture * texture = new Texture(size_, (Texture::FlagField) flagFields_);

	for(positive i = 0; i < Texture::FIELDS_COUNT; i++) {
		if(bitmaps_[i] != NULL) {
			Texture::Field field = (Texture::Field) i;
			switch(field) {
				case Texture::DIFFUSE_RGB:
				case Texture::EMISSIVE_RGB:
					colorBitmapToTexture(
					    bitmaps_[i], *texture, texture->getFieldOffset(field));
					break;
				case Texture::NORMAL_XYZ:
					normalBitmapToTexture(
					    bitmaps_[i], *texture, texture->getFieldOffset(field));
					break;
				case Texture::AMBIENT_I:
				case Texture::SPECULAR_I:
				case Texture::SHININESS_I:
					intensityBitmapToTexture(
					    bitmaps_[i], *texture, texture->getFieldOffset(field));
					break;
				default:
					cout << "Error: unknown field." << endl;
					break;
			}
		}
	}

	reset();

	return texture;
}

void TextureLoader::colorBitmapToTexture(
    SDL_Surface* bmp, Texture& texture, positive dataOffset)
{
	for(integer y = 0; y < (integer) bmp->h; y++) {
		for(integer x = 0; x < (integer) bmp->w; x++) {
			Uint8 r, g, b;
			getPixel(bmp, x, y, r, g, b);
			real* data           = texture.getData(x, y);
			data[dataOffset + 0] = (real) r / 255;
			data[dataOffset + 1] = (real) g / 255;
			data[dataOffset + 2] = (real) b / 255;
		}
	}
}

void TextureLoader::intensityBitmapToTexture(
    SDL_Surface* bmp, Texture& texture, positive dataOffset)
{
	for(integer y = 0; y < (integer) bmp->h; y++) {
		for(integer x = 0; x < (integer) bmp->w; x++) {
			Uint8 r, g, b;
			getPixel(bmp, x, y, r, g, b);
			real* data       = texture.getData(x, y);
			data[dataOffset] = (real) r / 255;
		}
	}
}

void TextureLoader::normalBitmapToTexture(
    SDL_Surface* bmp, Texture& texture, positive dataOffset)
{
	for(integer y = 0; y < (integer) bmp->h; y++) {
		for(integer x = 0; x < (integer) bmp->w; x++) {
			Uint8 nx, ny, nz;
			getPixel(bmp, x, y, nx, ny, nz);
			real* data           = texture.getData(x, y);
			data[dataOffset + 0] = ((real) nx / 255) * 2 - 1;
			data[dataOffset + 1] = ((real) ny / 255) * 2 - 1;
			data[dataOffset + 2] = ((real) nz / 255) * 2 - 1;
			normalize(data);
		}
	}
}

