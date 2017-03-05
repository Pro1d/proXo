#include "Sun.h"
#include "core/math/Vector.h"
#include "core/math/basics.h"
#include "core/model/Light.h"
#include <algorithm>
#include <limits>

SunLight::SunLight() : Light(true)
{
}

void SunLight::lighting(vec4 color, vec4 normal, vec4 point, real ambient,
    real diffuse, real specular, real shininess, vec4 colorOut)
{
	real L[4], R[4], V[4];

	// L: vector point->transformedPosition
	substract(transformedPosition, point, L);
	real distToLight = std::max((real) 0, dot(L, transformedDirection));

	// light is too far, attenuation makes it invisible
	if(transformedReductionFactor > 0
	    && distToLight * distToLight > transformedDistanceMax)
		return;

	// intensity reduction with distance
	real lightIntensityInv =
	    1 + distToLight * distToLight * transformedReductionFactor;

	// dot product for diffuse intensity
	real dirDotNormal = -dot(transformedDirection, normal);
	real D            = std::max((real) 0, dirDotNormal);

	// R: reflect of L with normal
	real N = 2 * dirDotNormal;
	R[0]   = N * normal[0] - -transformedDirection[0];
	R[1]   = N * normal[1] - -transformedDirection[1];
	R[2]   = N * normal[2] - -transformedDirection[2];
	R[3]   = 0;
	// V: normalized vector to camera
	normalize(point, V);
	// S: specular intensity
	real S = specularIntensity(-dot(R, V), shininess) * specular;
	// C: diffuse intensity
	real C = diffuse * D;

	// final intensity
	real A = intensity / lightIntensityInv; // todo premult lightColor*intensity
	C *= A;
	S *= A;

	// Resulting color
	colorOut[0] += ((color[0] * C) + S) * this->color[0];
	colorOut[1] += ((color[1] * C) + S) * this->color[1];
	colorOut[2] += ((color[2] * C) + S) * this->color[2];
}

real SunLight::getDirectionToSource(vec4 point, vec4 directionOut)
{
	directionOut[0] = -transformedDirection[0];
	directionOut[1] = -transformedDirection[1];
	directionOut[2] = -transformedDirection[2];
	directionOut[3] = 1;
	return std::numeric_limits<real>::infinity();
}
