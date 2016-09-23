#include "Buffer.h"

Buffer::Buffer(positive w, positive h) :
		data(new real[w*h*VEC4_SCALARS_COUNT]),
		width(w),
		height(h)
{

}

Buffer::~Buffer() {
	delete[] data;
}

void Buffer::clear() {
    integer i = width*height*VEC4_SCALARS_COUNT;
    while((i-=VEC4_SCALARS_COUNT) >= 0) {
        data[i|0] = 1;
        data[i|1] = 0;
        data[i|2] = 0;
        data[i|3] = 0;
    }
}
