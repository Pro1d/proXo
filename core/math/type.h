#ifndef __TYPE_H__
#define	__TYPE_H__

#include "../config/global.h"

#ifdef USE_DOUBLE_PRECISION_FLOATING_VALUE
typedef double real;
#else
typedef float real;
#endif

#ifdef USE_64_BITS_INTEGER_VALUE
typedef int integer;
typedef unsigned int positive;
#else
typedef long long integer;
typedef unsigned long long positive;
#endif


typedef vec2 real*; // real[2]
typedef vec3 real*; // real[3]
typedef vec4 real*; // real[4]
typedef mat4 real*; // real[4*4]
#define VEC2_SCALARS_COUNT   2
#define VEC2_SIZE           (sizeof(real)*VEC2_SCALARS_COUNT)
#define VEC3_SCALARS_COUNT   3
#define VEC3_SIZE           (sizeof(real)*VEC3_SCALARS_COUNT)
#define VEC4_SCALARS_COUNT   4
#define VEC4_SIZE           (sizeof(real)*VEC4_SCALARS_COUNT)
#define MAT4_SCALARS_COUNT	16
#define MAT4_SIZE			(sizeof(real)*MAT4_SCALARS_COUNT)

#endif
