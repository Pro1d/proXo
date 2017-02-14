#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "core/math/type.h"
#include "core/math/Vector.h"

real refractRay(vec3 incident, vec3 normal, real n1, real n2, vec3 refractOut);

#endif // __PHYSICS_H__
