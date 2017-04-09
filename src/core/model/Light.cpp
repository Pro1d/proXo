#include "Light.h"
#include "core/math/Matrix.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include <algorithm>
#include <limits>

namespace proxo {

Light::Light(bool castShadow)
    : color{ 1, 1, 1 },
      intensity(1),
      reductionFactor(0),
      distanceMax(0),
      direction{ 1, 0, 0, 0 },
      position{ 0, 0, 0, 1 },
      radius(0),
      castShadow(castShadow)
{
}

Light::~Light()
{
}

void Light::setDirection(vec3 dir)
{
	direction[0] = dir[0];
	direction[1] = dir[1];
	direction[2] = dir[2];
}

void Light::setPosition(vec3 pos)
{
	position[0] = pos[0];
	position[1] = pos[1];
	position[2] = pos[2];
}

void Light::setColor(vec3 c)
{
	color[0] = c[0];
	color[1] = c[1];
	color[2] = c[2];
}

void Light::setIntensity(real i)
{
	intensity = i;
	updateDistanceMax();
}

void Light::setReductionFactor(real i)
{
	reductionFactor = i;
	updateDistanceMax();
}
void Light::updateDistanceMax()
{
	real k = 1.0 / 255;
	if(reductionFactor > 0)
		distanceMax = (intensity - k) / (k * reductionFactor);
	else
		distanceMax = std::numeric_limits<real>::infinity();
}

real Light::getDistanceMax()
{
	return distanceMax;
}

void Light::setFallOff(real i)
{
	fallOff = cos(toRadians(i));
}

void Light::setCutOff(real i)
{
	cutOff = cos(toRadians(i));
}

void Light::setRadius(real r)
{
	radius = r;
}

void Light::transform(mat4 matrix)
{
	multiplyNoNormalizeMV(matrix, position, transformedPosition);
	multiplyNoNormalizeMV(matrix, direction, transformedDirection);
	normalize(transformedDirection);
	real squaredScale          = getMatrixSquaredScale(matrix);
	transformedDistanceMax     = distanceMax / squaredScale;
	transformedReductionFactor = reductionFactor * squaredScale;
	transformedRadius = radius * sqrt_inv(squaredScale);
}

real Light::specularIntensity(real RdotV, real shininess)
{
	return RdotV > 0 ? RdotV / (shininess - RdotV * shininess + RdotV) : 0;
	/*
	if(RdotV > (real) 0.9) {
	    real k = (1-shininess*(1-RdotV)/8);
	    k *= k;
	    k *= k;
	    return k * k;
	}
	else
	    return 0;*/
}

} // namespace proxo
