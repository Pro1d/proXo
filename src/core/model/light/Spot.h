#ifndef __SPOT_H__
#define __SPOT_H__

#include "core/math/type.h"
#include "core/model/Light.h"

namespace proxo {

class SpotLight : public Light {
public:
	SpotLight();
	void lighting(vec4 color, vec4 normal, vec4 point, real ambient,
	    real diffuse, real specular, real shininess, vec4 colorOut);
	real getDirectionToSource(vec4 point, vec4 directionOut);
};

} // namespace proxo

#endif // __SPOT_H__
