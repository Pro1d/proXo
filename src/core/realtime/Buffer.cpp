#include "Buffer.h"
#include <cstring>

Buffer::Buffer(positive w, positive h)
    : data(new real[w * h * VEC4_SCALARS_COUNT]), width(w), height(h)
{
}

Buffer::~Buffer()
{
	delete[] data;
}

void Buffer::clear()
{
	integer i = width * height * VEC4_SCALARS_COUNT;
	while((i -= VEC4_SCALARS_COUNT) >= 0) {
		data[i | 0] = 1;
		data[i | 1] = 0;
		data[i | 2] = 0;
		data[i | 3] = 0;
	}
}

void Buffer::clear(positive yStart, positive yEnd)
{
	positive i   = yStart * width * VEC4_SCALARS_COUNT;
	positive end = yEnd * width * VEC4_SCALARS_COUNT;

	while(i < end) {
		data[i]     = 1;
		data[i + 1] = 0;
		data[i + 2] = 0;
		data[i + 3] = 0;
		i += VEC4_SCALARS_COUNT;
	}
}

void Buffer::merge(Buffer& buffer)
{
	vec4 cursor = data;
	vec4 end    = data + width * height * VEC4_SCALARS_COUNT;
	vec4 source = buffer.data + width * height * VEC4_SCALARS_COUNT;

	while(cursor != end) {
		if(source[0] > cursor[0])
			memcpy(cursor, source, VEC4_SIZE);
		cursor += VEC4_SCALARS_COUNT;
	}
}
