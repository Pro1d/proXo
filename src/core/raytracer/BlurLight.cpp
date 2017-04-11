#include "BlurLight.h"
#include "core/math/Vector.h"
#include <algorithm>

namespace proxo {

BlurLight::Iterator BlurLight::set(vec3 direction, real distance, real radius)
{
	this->distance = distance;
	this->radius   = radius;
	T[0] = N[0] = direction[0];
	T[1] = N[1] = direction[1];
	T[2] = N[2] = direction[2];
	T[3] = N[3] = 0;
	if(T[0] == 0)
		T[0] = fabs(T[1]) + fabs(T[2]);
	else if(T[1] == 0)
		T[1] = fabs(T[0]) + fabs(T[2]);
	else
		T[2] = fabs(T[0]) + fabs(T[1]);
	cross(T, N, B);
	normalize(B);
	cross(B, N, T);

	if(distance <= 0 || radius <= 0)
		return BlurLight::Iterator(0, 0);
	return Iterator(
	    5, 2);//std::min((positive)(radius / distance * 500 + 1), (positive) 4));
}

void BlurLight::getDirection(const Iterator& it, vec3 outDir)
{
	if(it.radiusStepCount == 0 || it.radiusStep == 0) {
		outDir[0] = N[0];
		outDir[1] = N[1];
		outDir[2] = N[2];
	}
	else {
		real r                  = radius * it.radiusStep / it.radiusStepCount;
		positive angleStepCount = it.firstAngleStepCount * it.radiusStep;
		real x = cos(it.angleStep * 2 * M_PI / angleStepCount) * r;
		real y = sin(it.angleStep * 2 * M_PI / angleStepCount) * r;

		outDir[0] = T[0] * x + B[0] * y + N[0] * distance;
		outDir[1] = T[1] * x + B[1] * y + N[1] * distance;
		outDir[2] = T[2] * x + B[2] * y + N[2] * distance;
		normalize(outDir);
	}
}

} // namespace proxo
