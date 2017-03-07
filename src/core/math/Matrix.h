#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "basics.h"
#include "type.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace proxo {

mat4 newMatrix();
void identity(mat4 mOut);

void multiplyMM(mat4 A, mat4 B, mat4 mOut);
void multiplyMV(mat4 M, vec4 v, vec4 vOut);
void multiplyNoNormalizeMV(mat4 M, vec4 v, vec4 vOut);

void copyMatrix(mat4 dst, const mat4 src);

void applyRotate(mat4 mOut, real angle, real x, real y, real z);
void setRotateX(mat4 mOut, real angle);
void setRotateY(mat4 mOut, real angle);
void setRotateZ(mat4 mOut, real angle);
void setRotate(mat4 mOut, real angle, real x, real y, real z);

void applyTranslate(mat4, real x, real y, real z);
void setTranslate(mat4 mOut, real x, real y, real z);

void applyScale(mat4 mOut, real s);
void setScale(mat4 mOut, real x, real y, real z);
real getMatrixScale(mat4 m);
real getMatrixSquaredScale(mat4 m);

void printMatrix(mat4 m, positive tabCount);

inline mat4 newMatrix()
{
	mat4 m = new real[MAT4_SCALARS_COUNT];
	identity(m);
	return m;
}

inline void identity(mat4 mOut)
{
	mOut[0] = 1;
	mOut[1] = 0;
	mOut[2] = 0;
	mOut[3] = 0;

	mOut[4] = 0;
	mOut[5] = 1;
	mOut[6] = 0;
	mOut[7] = 0;

	mOut[8]  = 0;
	mOut[9]  = 0;
	mOut[10] = 1;
	mOut[11] = 0;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline void multiplyMM(mat4 A, mat4 B, mat4 mOut)
{
	mOut[0] = A[0] * B[0] + A[1] * B[4] + A[2] * B[8] + A[3] * B[12];
	mOut[1] = A[0] * B[1] + A[1] * B[5] + A[2] * B[9] + A[3] * B[13];
	mOut[2] = A[0] * B[2] + A[1] * B[6] + A[2] * B[10] + A[3] * B[14];
	mOut[3] = A[0] * B[3] + A[1] * B[7] + A[2] * B[11] + A[3] * B[15];

	mOut[4] = A[4] * B[0] + A[5] * B[4] + A[6] * B[8] + A[7] * B[12];
	mOut[5] = A[4] * B[1] + A[5] * B[5] + A[6] * B[9] + A[7] * B[13];
	mOut[6] = A[4] * B[2] + A[5] * B[6] + A[6] * B[10] + A[7] * B[14];
	mOut[7] = A[4] * B[3] + A[5] * B[7] + A[6] * B[11] + A[7] * B[15];

	mOut[8]  = A[8] * B[0] + A[9] * B[4] + A[10] * B[8] + A[11] * B[12];
	mOut[9]  = A[8] * B[1] + A[9] * B[5] + A[10] * B[9] + A[11] * B[13];
	mOut[10] = A[8] * B[2] + A[9] * B[6] + A[10] * B[10] + A[11] * B[14];
	mOut[11] = A[8] * B[3] + A[9] * B[7] + A[10] * B[11] + A[11] * B[15];

	mOut[12] = A[12] * B[0] + A[13] * B[4] + A[14] * B[8] + A[15] * B[12];
	mOut[13] = A[12] * B[1] + A[13] * B[5] + A[14] * B[9] + A[15] * B[13];
	mOut[14] = A[12] * B[2] + A[13] * B[6] + A[14] * B[10] + A[15] * B[14];
	mOut[15] = A[12] * B[3] + A[13] * B[7] + A[14] * B[11] + A[15] * B[15];
}

inline void multiplyMV(mat4 M, vec4 v, vec4 vOut)
{
	vOut[0]     = v[0] * M[0] + v[1] * M[1] + v[2] * M[2] + v[3] * M[3];
	vOut[1]     = v[0] * M[4] + v[1] * M[5] + v[2] * M[6] + v[3] * M[7];
	vOut[2]     = v[0] * M[8] + v[1] * M[9] + v[2] * M[10] + v[3] * M[11];

  // TODO remove fabs()
	real vOut_3 = fabs(v[0] * M[12] + v[1] * M[13] + v[2] * M[14]
	    + v[3] * M[15]); 
	
  vOut[0] /= vOut_3;
	vOut[1] /= vOut_3;
	vOut[2] /= vOut_3;
	vOut[3] = (real) 1;
}

inline void multiplyNoNormalizeMV(mat4 M, vec4 v, vec4 vOut)
{
	vOut[0] = v[0] * M[0] + v[1] * M[1] + v[2] * M[2] + v[3] * M[3];
	vOut[1] = v[0] * M[4] + v[1] * M[5] + v[2] * M[6] + v[3] * M[7];
	vOut[2] = v[0] * M[8] + v[1] * M[9] + v[2] * M[10] + v[3] * M[11];
	vOut[3] = v[0] * M[12] + v[1] * M[13] + v[2] * M[14] + v[3] * M[15];
}

inline void copyMatrix(mat4 dst, const mat4 src)
{
	memcpy(dst, src, MAT4_SIZE);
}

inline void applyRotate(mat4 mOut, real angle, real x, real y, real z)
{
  real _tmpMatrix1[MAT4_SCALARS_COUNT];
  real _tmpMatrix2[MAT4_SCALARS_COUNT];

	setRotate(_tmpMatrix1, angle, x, y, z);
	multiplyMM(_tmpMatrix1, mOut, _tmpMatrix2);
	memcpy(mOut, _tmpMatrix2, MAT4_SIZE);
}

inline void setRotateX(mat4 mOut, real angle)
{
	real c = cos(angle), s = sin(angle);
	mOut[0] = 1;
	mOut[1] = 0;
	mOut[2] = 0;
	mOut[3] = 0;

	mOut[4] = 0;
	mOut[5] = c;
	mOut[6] = -s;
	mOut[7] = 0;

	mOut[8]  = 0;
	mOut[9]  = s;
	mOut[10] = c;
	mOut[11] = 0;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline void setRotateY(mat4 mOut, real angle)
{
	real c = cos(angle), s = sin(angle);
	mOut[0] = c;
	mOut[1] = 0;
	mOut[2] = s;
	mOut[3] = 0;

	mOut[4] = 0;
	mOut[5] = 1;
	mOut[6] = 0;
	mOut[7] = 0;

	mOut[8]  = -s;
	mOut[9]  = 0;
	mOut[10] = c;
	mOut[11] = 0;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline void setRotateZ(mat4 mOut, real angle)
{
	real c = cos(angle), s = sin(angle);
	mOut[0] = c;
	mOut[1] = -s;
	mOut[2] = 0;
	mOut[3] = 0;

	mOut[4] = s;
	mOut[5] = c;
	mOut[6] = 0;
	mOut[7] = 0;

	mOut[8]  = 0;
	mOut[9]  = 0;
	mOut[10] = 1;
	mOut[11] = 0;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline void setRotate(mat4 mOut, real angle, real x, real y, real z)
{
	real c = cos(angle), s = sin(angle);

	mOut[0] = c + x * x * (1 - c);
	mOut[1] = x * y * (1 - c) - z * s;
	mOut[2] = x * z * (1 - c) + y * s;
	mOut[3] = 0;

	mOut[4] = x * y * (1 - c) + z * s;
	mOut[5] = c + y * y * (1 - c);
	mOut[6] = y * z * (1 - c) - x * s;
	mOut[7] = 0;

	mOut[8]  = x * z * (1 - c) - y * s;
	mOut[9]  = y * z * (1 - c) + x * s;
	mOut[10] = c + z * z * (1 - c);
	mOut[11] = 0;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline void applyTranslate(mat4 mOut, real x, real y, real z)
{
  real _tmpMatrix1[MAT4_SCALARS_COUNT];
  real _tmpMatrix2[MAT4_SCALARS_COUNT];

	setTranslate(_tmpMatrix1, x, y, z);
	multiplyMM(_tmpMatrix1, mOut, _tmpMatrix2);
	memcpy(mOut, _tmpMatrix2, MAT4_SIZE);
}

inline void setTranslate(mat4 mOut, real x, real y, real z)
{
	mOut[0] = 1;
	mOut[1] = 0;
	mOut[2] = 0;
	mOut[3] = x;

	mOut[4] = 0;
	mOut[5] = 1;
	mOut[6] = 0;
	mOut[7] = y;

	mOut[8]  = 0;
	mOut[9]  = 0;
	mOut[10] = 1;
	mOut[11] = z;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline void applyScale(mat4 mOut, real s)
{
  real _tmpMatrix1[MAT4_SCALARS_COUNT];
  real _tmpMatrix2[MAT4_SCALARS_COUNT];

	setScale(_tmpMatrix1, s, s, s);
	multiplyMM(_tmpMatrix1, mOut, _tmpMatrix2);
	memcpy(mOut, _tmpMatrix2, MAT4_SIZE);
}

inline void setScale(mat4 mOut, real x, real y, real z)
{
	mOut[0] = x;
	mOut[1] = 0;
	mOut[2] = 0;
	mOut[3] = 0;

	mOut[4] = 0;
	mOut[5] = y;
	mOut[6] = 0;
	mOut[7] = 0;

	mOut[8]  = 0;
	mOut[9]  = 0;
	mOut[10] = z;
	mOut[11] = 0;

	mOut[12] = 0;
	mOut[13] = 0;
	mOut[14] = 0;
	mOut[15] = 1;
}

inline real getMatrixScale(mat4 m)
{
	real sum = getMatrixSquaredScale(m);

	return sum * sqrt_inv(sum);
}

inline real getMatrixSquaredScale(mat4 m)
{
	real sum = m[0] * m[0] + m[1] * m[1] + m[2] * m[2];
	sum += m[4] * m[4] + m[5] * m[5] + m[6] * m[6];
	sum += m[8] * m[8] + m[9] * m[9] + m[10] * m[10];

	return sum;
}

inline void printMatrix(mat4 m, positive tabCount)
{
	for(positive t = 0; t < tabCount; t++)
		printf("\t");
	printf("@%p\n", m);
	for(positive i = 0; i < 4; i++) {
		for(positive t = 0; t < tabCount; t++)
			printf("\t");
		for(positive j = 0; j < 4; j++)
			printf("%f\t", m[j + i * 4]);
		printf("\n");
	}
}

} // namespace proxo

#endif
