#ifndef __SUN_H__
#define __SUN_H__

#include "core/model/Light.h"
#include "core/math/type.h"

class SunLight : public Light {
public:
    SunLight();
    void lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut);
    real getDirectionToSource(vec4 point, vec4 directionOut);
};

#endif // __SUN_H__
