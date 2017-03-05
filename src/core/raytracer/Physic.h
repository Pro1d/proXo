#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "core/math/Vector.h"
#include "core/math/type.h"

namespace proxo {

real refractRay(vec3 incident, vec3 normal, real n1, real n2, vec3 refractOut);

} // namespace proxo

#endif // __PHYSICS_H__
