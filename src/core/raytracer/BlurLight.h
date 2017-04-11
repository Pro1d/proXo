#ifndef __BLUR_LIGHT_H__
#define __BLUR_LIGHT_H__

#include "core/math/type.h"

namespace proxo {

class BlurLight {
public:
	class Iterator {
	public:
		Iterator(positive a, positive r)
		    : firstAngleStepCount(a),
		      radiusStepCount(r),
		      angleStep(0),
		      radiusStep(0)
		{
		}
		bool end()
		{
			return radiusStep > radiusStepCount;
		}
		void incr()
		{
			angleStep++;
			if(angleStep >= firstAngleStepCount * radiusStep) {
				angleStep = 0;
				radiusStep++;
			}
		}
		const positive firstAngleStepCount, radiusStepCount;
		positive angleStep, radiusStep;
	};

	Iterator set(vec3 direction, real distance, real radius);
	void getDirection(const Iterator& it, vec3 outDir);

private:
	real distance, radius;
	real N[VEC4_SCALARS_COUNT], T[VEC4_SCALARS_COUNT], B[VEC4_SCALARS_COUNT];
};

} // namespace proxo

#endif

