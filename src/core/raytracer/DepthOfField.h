
#ifndef __DEPTH_OF_FIELD_H__
#define __DEPTH_OF_FIELD_H__

#include <core/math/type.h>
#include <vector>

namespace proxo {

class DepthOfField {
public:
	typedef std::vector<std::pair<real, real>> OriginsArray;
	typedef OriginsArray::const_iterator OriginsIterator;

	DepthOfField();

	void setFocusDistance(real d);
	void setAperture(real a);

	void updatePattern(positive firstAngleStepCount, positive radiusStepCount);

	OriginsIterator begin();
	OriginsIterator end();

	void getDirection(
	    vec3 mainDir, OriginsIterator oi, vec4 outDir, vec4 outOrig);

private:
	real focusDistance_;
	real aperture_;
	OriginsArray origins_;
};

} // namespace proxo

#endif

