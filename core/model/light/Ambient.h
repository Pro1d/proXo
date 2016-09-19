#ifndef __AMBIENT_H__
#define __AMBIENT_H__

#include "../Light.h"
#include "../../math/type.h"

class AmbientLight : public Light {
public:
    void lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut);
};

#endif // __AMBIENT_H__

