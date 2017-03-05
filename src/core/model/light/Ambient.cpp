#include "Ambient.h"
#include "core/math/Vector.h"
#include "core/math/basics.h"
#include "core/model/Light.h"
#include <limits>

namespace proxo {

AmbientLight::AmbientLight() : Light(false)
{
}

void AmbientLight::lighting(vec4 color, vec4 normal, vec4 point, real ambient,
    real diffuse, real specular, real shininess, vec4 colorOut)
{
	// Ambient
	real A = ambient * intensity;

	if(reductionFactor > 0) {
		real L[4];

		// L: vector point->transformedPosition
		substract(transformedPosition, point, L);
		real distToLight2 = squaredLength(L);

		// light is too far, attenuation makes it invisible
		if(transformedReductionFactor > 0
		    && distToLight2 > transformedDistanceMax)
			return;

		// intensity reduction with distance
		real lightIntensityInv = 1 + distToLight2 * transformedReductionFactor;

		// final intensity
		A /= lightIntensityInv; // todo premult lightColor*intensity
	}

	// Resulting color
	colorOut[0] += (color[0] * A) * this->color[0];
	colorOut[1] += (color[1] * A) * this->color[1];
	colorOut[2] += (color[2] * A) * this->color[2];
}

real AmbientLight::getDirectionToSource(vec4 point, vec4 directionOut)
{
	return std::numeric_limits<real>::infinity();
}

} // namespace proxo
