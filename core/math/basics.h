#ifndef __MATH_BASICS_H__
#define __MATH_BASICS_H__

#include "type.h"

#include "inv_sqrt.h"

real clamp01(real x);

void normalize(vec3 v);

real clamp(real x, real low, real high);

//real triangleArea(vec2 a, vec2 b, vec2 c);

void faceNormal(vec3 a, vec3 b, vec3 c, vec3 normalOut);

#endif
