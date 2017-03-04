#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "core/math/type.h"


class Texture {
	public:
		Texture(vec4 data, positive size);

		inline vec4 getValue(integer u, integer v) {
			return data_ + ((u & sizeMask_) | ((v & sizeMask_) << log2size_)) * VEC4_SCALARS_COUNT;
		}
		inline vec4 getValue(real u, real v) {
			return getValue((integer) (u * size_), (integer) (v * size_));
		}

	private:
		vec4 data_;
		positive size_;
		positive log2size_;
		positive sizeMask_;
};

#endif
