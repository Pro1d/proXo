#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "core/math/type.h"

namespace proxo {

/**
 * Access data:
 * if(hasField(DIFFUSE_RGB))
 * 	color = (vec3) getData(u, v) + getFieldOffset(DIFFUSE_RGB);
 */
class Texture {
public:
	enum Field {
		DIFFUSE_RGB,
		AMBIENT_I,
		SPECULAR_I,
		EMISSIVE_RGB,
		NORMAL_XYZ,
		SHININESS_I,
		FIELDS_COUNT
	};
	enum FlagField {
		FLAG_DIFFUSE_RGB  = 1 << DIFFUSE_RGB,
		FLAG_AMBIENT_I    = 1 << AMBIENT_I,
		FLAG_SPECULAR_I   = 1 << SPECULAR_I,
		FLAG_EMISSIVE_RGB = 1 << EMISSIVE_RGB,
		FLAG_NORMAL_XYZ   = 1 << NORMAL_XYZ,
		FLAG_SHININESS_I  = 1 << SHININESS_I,
		MASK_FIELDS       = (1 << FIELDS_COUNT) - 1
	};
	static const positive FieldSize[FIELDS_COUNT];

	// the size must be a power of 2 (see Texture::isSizeValid)
	Texture(positive size, FlagField flagFields = FLAG_DIFFUSE_RGB);
	~Texture();

	inline real* getData(integer u, integer v)
	{
		return data_ + (((u & sizeMask_) | ((v & sizeMask_) << log2size_))
		                   << log2depth_);
	}

	inline real* getData(real u, real v)
	{
		return getData((integer)(u * sizeReal_), (integer)(v * sizeReal_));
	}

	inline bool hasField(FlagField field) { return (flagFields_ & field) != 0; }

	inline bool hasField(Field field)
	{
		return hasField((FlagField)(1 << field));
	}

	inline positive getFieldOffset(Field field) { return offsets_[field]; }

	inline positive getSize() { return size_; }

	static bool isSizeValid(positive size);

	static void normalMapToWorldCoord(vec3 pA, vec3 pB, vec3 pC, vec2 tA, vec2 tB, vec2 tC, vec3 normalIn, vec3 normalOut);

private:
	real* data_;
	positive size_;
	real sizeReal_;
	positive log2size_;
	positive sizeMask_;
	positive log2depth_;
	FlagField flagFields_;
	positive offsets_[FIELDS_COUNT];
};

} // namespace proxo

#endif
