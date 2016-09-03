#include "Buffer.h"

Buffer::Buffer(positive w, positive h) :
		data(new real[w*h*4]),
		width(w),
		height(h)
{
	
}

Buffer::~Buffer() {
	delete[] data;
}

inline real * Buffer::getPtr(positive x, positive y) {
	return data + (x + y * width) * 4;
}