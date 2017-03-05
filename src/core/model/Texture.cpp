#include "Texture.h"
#include <iostream>

using namespace std;

namespace proxo {

Texture::Texture(vec4 data, positive size)
    : data_(data), size_(size), log2size_(0), sizeMask_(size - 1)
{
	while(size >>= 1)
		log2size_++;
}

bool Texture::isSizeValid(positive size)
{
	// size must be a power of 2 <=> size must have only one bit set to 1
	// Here is a cute hack to verify it
	return size > 0 && (size & (~size + 1)) == size;
}

} // namespace proxo
