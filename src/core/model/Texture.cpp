#include "Texture.h"
#include <iostream>

using namespace std;

Texture::Texture(vec4 data, positive size) : data_(data), size_(size), log2size_(0)
{
	while(((positive) 1 << log2size_) < size_)
		log2size_++;

	positive correct_size = (positive) 1 << log2size_;
	sizeMask_             = correct_size - 1;

	if(size_ != correct_size) {
		size_ = correct_size;
		cerr << "Error: texture size must be power of 2." << endl;
	}
}
