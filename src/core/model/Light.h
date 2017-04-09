#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "core/math/type.h"

namespace proxo {

class Light {
public:
	Light(bool castShadow);
	virtual ~Light();
	void initialize();
	void transform(mat4 matrix);
	virtual void lighting(vec4 color, vec4 normal, vec4 point, real ambient,
	    real diffuse, real specular, real shininess, vec4 colorOut) = 0;
	virtual real getDirectionToSource(vec4 point, vec4 directionOut) = 0;
	void setDirection(vec3 dir);
	void setPosition(vec3 pos);
	void setColor(vec3 c);
	void setIntensity(real i);
	void setReductionFactor(real i);
	void updateDistanceMax();
	real getDistanceMax();
	void setFallOff(real i);
	void setCutOff(real i);
	void setRadius(real r);

protected:
	real specularIntensity(real RdotV, real shininess);

	real transformedPosition[VEC4_SCALARS_COUNT];
	real transformedDirection[VEC4_SCALARS_COUNT];
	real transformedReductionFactor;
	real transformedDistanceMax;
	real color[VEC4_SCALARS_COUNT];
	real intensity;
	real reductionFactor; //  1 / (d^2 * factor + 1);
	real distanceMax;
	real fallOff, cutOff;
	real direction[VEC4_SCALARS_COUNT];

public:
	// Radius of the sphere emitting light (for soft shadow)
	real position[VEC4_SCALARS_COUNT];
	real radius;
	real transformedRadius;
	bool castShadow;
};

} // namespace proxo

#endif // __LIGHT_H__
