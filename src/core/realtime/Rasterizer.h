#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "core/common/Buffer.h"
#include "core/math/type.h"

namespace proxo {

void triangle(Buffer& buffer, const vec3 A, const vec3 B, const vec3 C,
    const vec3 colorA, const vec3 colorB, const vec3 colorC);

} // namespace proxo

#endif
