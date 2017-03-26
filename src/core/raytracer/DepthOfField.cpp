#include "DepthOfField.h"
#include "core/math/Vector.h"
#include <cmath>

#include <iostream>
namespace proxo {

DepthOfField::DepthOfField() : focusDistance_(1), aperture_(0)
{
}

void DepthOfField::setFocusDistance(real d)
{
	focusDistance_ = d;
}

void DepthOfField::setAperture(real a)
{
	aperture_ = a;
}

void DepthOfField::updatePattern(
    positive firstAngleStepCount, positive radiusStepCount)
{
	if(aperture_ <= 0 || radiusStepCount == 0 || firstAngleStepCount == 0) {
		origins_.clear();
	}
	else {
		origins_.resize(
		    firstAngleStepCount * radiusStepCount * (radiusStepCount + 1) / 2);
		positive i = 0;
		for(positive r = 1; r <= radiusStepCount; r++) {
			positive angleStepCount = firstAngleStepCount * r;
			real radius             = r * aperture_ / radiusStepCount;

			for(positive a = 0; a < angleStepCount; a++) {
				origins_[i].first = cos(a * 2 * M_PI / angleStepCount) * radius;
				origins_[i].second =
				    sin(a * 2 * M_PI / angleStepCount) * radius;
				i++;
			}
		}
	}
}

DepthOfField::OriginsIterator DepthOfField::begin()
{
	return origins_.begin();
}

DepthOfField::OriginsIterator DepthOfField::end()
{
	return origins_.end();
}

positive DepthOfField::getRaysCount()
{
	return origins_.size();
}

void DepthOfField::getDirection(
    vec3 mainDir, OriginsIterator oi, vec4 outDir, vec4 outOrig)
{
	outOrig[0] = oi->first;
	outOrig[1] = oi->second;
	outOrig[2] = 0;
	outOrig[3] = 1;

	real focusPoint[VEC4_SCALARS_COUNT] = { mainDir[0] * focusDistance_,
		mainDir[1] * focusDistance_, mainDir[2] * focusDistance_, 1 };

	substract(focusPoint, outOrig, outDir);
	normalize(outDir);
}

} // namespace proxo
