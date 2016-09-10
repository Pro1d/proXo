#include "basics.h"
#include "type.h"

inline real clamp01(real x) {
	if(x <= (real) 0)
		return (real) 0;
	if(x >= (real) 1)
		return (real) 1;
	return x;
}

inline real clamp(real x, real low, real high) {
	if(x <= low)
		return low;
	if(x >= high)
		return high;
	return x;
}

inline void normalize(vec3 v) {
	real inv_norm = sqrt_inv(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] *= inv_norm;
	v[1] *= inv_norm;
	v[2] *= inv_norm;
}

/*inline real triangleArea(vec2 a, vec2 b, vec2 c) {
	real abx = b[0]-a[0], aby = b[1]-a[1];
    real acx = c[0]-a[0], acy = c[1]-a[1];
    real cbx = b[0]-c[0], cby = b[1]-c[1];
    real ab = abx*abx + aby*aby;
    real ac = acx*acx + acy*acy;
    real cb = cbx*cbx + cby*cby;

    if(ab > ac) {
        if(cb > ab)
            return abs(cbx*acy-acx*cby) / 2;
        else
            return abs(abx*acy-acx*aby) / 2;

    } else {
        if(cb > ac)
            return abs(cbx*acy-acx*cby) / 2;
        else
            return abs(abx*acy-acx*aby) / 2;
    }
}*/

inline void faceNormal(vec3 a, vec3 b, vec3 c, vec3 normalOut);
