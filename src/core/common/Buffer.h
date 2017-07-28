#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "core/math/type.h"

namespace proxo {

enum {
BUF_Z_OFFSET=0,
BUF_R_OFFSET=1,
BUF_G_OFFSET=2,
BUF_B_OFFSET=3
};

class Buffer {
public:
	Buffer(positive w, positive h);
	~Buffer();
	void clear();
	void clear(positive yStart, positive yEnd);
	void merge(Buffer& buffer);

	vec4 getPtr(positive x, positive y);

	vec4 data;
	positive width;
	positive height;
};

inline vec4 Buffer::getPtr(positive x, positive y)
{
	return data + (x + y * width) * VEC4_SCALARS_COUNT;
}

} // namespace proxo

#endif
