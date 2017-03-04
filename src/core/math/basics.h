#ifndef __MATH_BASICS_H__
#define __MATH_BASICS_H__

#include "type.h"

#include "inv_sqrt.h"
#define PI 3.1415926535897932384626433832795

real clamp01(real x);
real clamp(real x, real low, real high);
// real triangleArea(vec2 a, vec2 b, vec2 c);
void faceNormal(vec3 a, vec3 b, vec3 c, vec3 normalOut);

inline real toRadians(real deg)
{
	return deg * PI / 180;
}

inline real clamp01(real x)
{
	return clamp(x, 0, 1);
}

inline real clamp(real x, real low, real high)
{
	return (x <= low) ? low : ((x >= high) ? high : x);
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

#endif
