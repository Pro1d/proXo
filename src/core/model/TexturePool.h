#infdef __TEXTURE_POOL_H__
#define __TEXTURE_POOL_H__

class TexturePool {
	extern inline Texture* getTexture(positive id);
	Texture* textures;
	int texturesCount;
};

#endif