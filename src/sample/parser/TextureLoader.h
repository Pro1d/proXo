#ifndef __TEXTURE_LOADER_H__
#define __TEXTURE_LOADER_H__

#include "core/model/Texture.h"
#include <string>

class TextureLoader {
public:
	void reset() {}
	void addImageFile(const std::string& filename, proxo::Texture::Field field) {}
	proxo::Texture* createTexture() { return NULL; }

};

#endif
