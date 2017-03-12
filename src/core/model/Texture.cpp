#include "Texture.h"
#include "core/math/Vector.h"
#include <iostream>

using namespace std;

namespace proxo {

const positive Texture::FieldSize[FIELDS_COUNT] = { 3, 1, 1, 3, 3, 1 };

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

void Texture::normalMapToWorldCoord(vec3 pA, vec3 pB, vec3 pC, vec2 tA, vec2 tB, vec2 tC,
    vec3 normalIn, vec4 normalOut)
{
	// Vector AB
	real vAB[VEC3_SCALARS_COUNT] = {
		pB[0] - pA[0],
		pB[1] - pA[1],
		pB[2] - pA[2]
	};

	// Vector AC
	real vAC[VEC3_SCALARS_COUNT] = {
		pC[0] - pA[0],
		pC[1] - pA[1],
		pC[2] - pA[2]
	};

	real tAB[VEC2_SCALARS_COUNT] = { tB[0] - tA[0], tB[1] - tA[1] };
	real tAC[VEC2_SCALARS_COUNT] = { tC[0] - tA[0], tC[1] - tA[1] };
	real det = 1.0f / (tAB[0] * tAC[1] - tAC[0] * tAB[1]);

	// Tangente
	real T[VEC3_SCALARS_COUNT] = {
		det * (tAC[1] * vAB[0] - tAB[1] * vAC[0]),
		det * (tAC[1] * vAB[1] - tAB[1] * vAC[1]),
		det * (tAC[1] * vAB[2] - tAB[1] * vAC[2])
	};
	normalize(T);

	// Binormal
	real B[VEC3_SCALARS_COUNT] = {
		det * (tAB[0] * vAC[0] - tAC[0] * vAB[0]),
		det * (tAB[0] * vAC[1] - tAC[0] * vAB[1]),
		det * (tAB[0] * vAC[2] - tAC[0] * vAB[2])
	};
	normalize(B);

	// Normal
	real N[VEC4_SCALARS_COUNT];
	cross(vAB, vAC, N);
	normalize(N);

	// Output
	normalOut[0] = normalIn[0] * T[0] + normalIn[1] * B[0] + normalIn[2] * N[0];
	normalOut[1] = normalIn[0] * T[1] + normalIn[1] * B[1] + normalIn[2] * N[1];
	normalOut[2] = normalIn[0] * T[2] + normalIn[1] * B[2] + normalIn[2] * N[2];
	normalOut[3] = 0;
	normalize(normalOut);
}

} // namespace proxo
