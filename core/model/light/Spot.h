#ifndef __SPOT_H__
#define __SPOT_H__

#include "../Light.h"
#include "../../math/type.h"

class SpotLight : public Light {
public:
    void lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut);
};

#endif // __SPOT_H__

