#include "Physic.h"
#include "../math/type.h"
#include "../math/Vector.h"

real refractRay(vec3 incident, vec3 normal, real n1, real n2, vec3 refractOut) {
    real e = n1/n2;
    real dotI = -(incident[0]*normal[0] + incident[1]*normal[1] + incident[2]*normal[2]);
    real dotT2 = 1 - e*e*(1 - dotI*dotI);
    if(dotT2 <= 0)
        return 0.0f;

    real dotT = dotT2 * sqrt_inv(dotT2);

    refractOut[0] = e*incident[0] + (e*dotI - dotT)*normal[0];
    refractOut[1] = e*incident[1] + (e*dotI - dotT)*normal[1];
    refractOut[2] = e*incident[2] + (e*dotI - dotT)*normal[2];

    /// Fresnel equation for transmitance ratio
    real cosR = -(refractOut[0]*normal[0] + refractOut[1]*normal[1] + refractOut[2]*normal[2]);
    real cosI = dotI;

    real Rn = (n1*cosI-n2*cosR) / (n1*cosI+n2*cosR);
    real Rt = (n2*cosI-n1*cosR) / (n2*cosI+n1*cosR);

    return clamp01(1 - (Rn*Rn + Rt*Rt) / 2);
}
