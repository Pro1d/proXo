#include "Texture.h"
#include <iostream>

using namespace std;

namespace proxo {

const positive Texture::FieldSize[FIELDS_COUNT] = { 3, 3, 3, 3, 3, 1 };

Texture::Texture(positive size, FlagField fields)
    : data_(NULL), size_(size), sizeReal_((real) size), log2size_(0),
      sizeMask_(size - 1), log2depth_(0), flagFields_(fields)
{
	while(size >>= 1)
		log2size_++;

	positive offset = 0;
	for(positive f = 0; f < FIELDS_COUNT; f++) {
		offsets_[f] = offset;
		if(flagFields_ & (1 << f)) {
			offset += FieldSize[f];
		}
	}

	positive depth = offset;
	while(depth > ((positive) 1 << log2depth_))
		log2depth_++;

	data_ = new real[size_ * size_ * ((positive) 1 << log2depth_)];
}

Texture::~Texture()
{
	delete[] data_;
}

bool Texture::isSizeValid(positive size)
{
	// size must be a power of 2 <=> size must have only one bit set to 1
	// Here is a cute hack to verify it
	return size > 0 && (size & (~size + 1)) == size;
}

} // namespace proxo
