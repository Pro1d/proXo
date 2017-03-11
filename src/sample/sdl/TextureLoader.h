#ifndef __TEXTURE_LOADER_H__
#define __TEXTURE_LOADER_H__

#include "core/math/type.h"
#include "core/model/Texture.h"
#include <SDL/SDL.h>
#include <string>

class TextureLoader {
public:
	TextureLoader();
	~TextureLoader();
	void reset();
	void addImageFile(const std::string& filename, proxo::Texture::Field field);
	proxo::Texture* createTexture();

protected:
	SDL_Surface* loadBitmap(const std::string& filename);
	void colorBitmapToTexture(
	    SDL_Surface* bmp, proxo::Texture& texture, proxo::positive dataOffset);

	void intensityBitmapToTexture(
	    SDL_Surface* bmp, proxo::Texture& texture, proxo::positive dataOffset);

	void normalBitmapToTexture(
	    SDL_Surface* bmp, proxo::Texture& texture, proxo::positive dataOffset);

private:
	proxo::positive size_;
	proxo::positive flagFields_;
	SDL_Surface* bitmaps_[proxo::Texture::FIELDS_COUNT];
};

proxo::Texture* loadTextureFromBitmap(std::string const& filename,
    proxo::Texture::Field field, proxo::Texture* texture = NULL);

#endif

