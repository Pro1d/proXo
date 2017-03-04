#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "basics.h"
#include "type.h"
real dot(vec4 a, vec4 b);
real cross(vec4 a, vec4 b);
void normalize(vec4 in, vec4 out);
void normalize(vec3 v);
void reflect(vec4 i, vec4 normal, vec4 reflectOut);
real squaredLength(vec3 a);
void substract(vec4 a, vec4 b, vec4 out);
void multiply(vec4 a, real k);
bool isFaceOrientationZPositive(vec4 A, vec4 B, vec4 C);

inline void normalize(vec3 v)
{
	real inv_norm = sqrt_inv(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= inv_norm;
	v[1] *= inv_norm;
	v[2] *= inv_norm;
}

inline real dot(vec4 a, vec4 b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline void cross(vec4 a, vec4 b, vec4 r)
{
	r[0] = a[1] * b[2] - b[1] * a[2];
	r[1] = a[2] * b[0] - b[2] * a[0];
	r[2] = a[0] * b[1] - b[0] * a[1];
	r[3] = 0;
}

inline void normalize(vec4 in, vec4 out)
{
	real norm2   = squaredLength(in);
	real normInv = sqrt_inv(norm2);
	out[0]       = in[0] * normInv;
	out[1]       = in[1] * normInv;
	out[2]       = in[2] * normInv;
	out[3]       = 0;
}

inline void reflect(vec4 i, vec4 normal, vec4 reflectOut)
{
	real k = 2 * dot(i, normal);
	// r = i - 2*(i.n)*n
	reflectOut[0] = i[0] - k * normal[0];
	reflectOut[1] = i[1] - k * normal[1];
	reflectOut[2] = i[2] - k * normal[2];
	reflectOut[3] = 0;
}

inline real squaredLength(vec3 a)
{
	return a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
}

inline void substract(vec4 a, vec4 b, vec4 out)
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
	out[3] = 0;
}

inline void multiply(vec4 a, real k)
{
	a[0] *= k;
	a[1] *= k;
	a[2] *= k;
}

inline bool isFaceOrientationZPositive(vec4 A, vec4 B, vec4 C)
{
	return (B[0] - A[0]) * (C[1] - A[1]) - (C[0] - A[0]) * (B[1] - A[1]) > 0;
}

#endif // __VECTOR_H__
