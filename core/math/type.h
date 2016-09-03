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


typedef vec3 real*; // real[3]
typedef vec4 real*; // real[4]
typedef mat4 real*; // real[4*4]
#define MAT4_SCALARS_COUNT	16
#define MAT4_SIZE			(sizeof(real)*MAT4_SCALARS_COUNT)

#endif