#ifndef __POINT_H__
#define __POINT_H__

#include "core/math/type.h"
#include "core/model/Light.h"

namespace proxo {

class PointLight : public Light {
public:
	PointLight();
	void lighting(vec4 color, vec4 normal, vec4 point, real ambient,
	    real diffuse, real specular, real shininess, vec4 colorOut);
	real getDirectionToSource(vec4 point, vec4 directionOut);
};

} // namespace proxo

#endif // __POINT_H__
