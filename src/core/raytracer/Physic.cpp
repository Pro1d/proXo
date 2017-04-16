#include "Physic.h"
#include "core/math/Vector.h"
#include "core/math/type.h"

namespace proxo {

real refractRay(vec3 incident, vec3 normal, real n1, real n2, vec3 refractOut)
{
	real e    = n1 / n2;
	real dotI = -(incident[0] * normal[0] + incident[1] * normal[1]
	    + incident[2] * normal[2]);

	// Total reflection
	real dotT2 = 1 - e * e * (1 - dotI * dotI);
	if(dotT2 <= 0)
		return 0.0f;

	real dotT = dotT2 * sqrt_inv(dotT2);

	// Refract direction
	refractOut[0] = e * incident[0] + (e * dotI - dotT) * normal[0];
	refractOut[1] = e * incident[1] + (e * dotI - dotT) * normal[1];
	refractOut[2] = e * incident[2] + (e * dotI - dotT) * normal[2];

	/// Fresnel equation for transmitance ratio
	// The coefficient is the same if n1 and n2 are swapped
	real cosR = -(refractOut[0] * normal[0] + refractOut[1] * normal[1]
	    + refractOut[2] * normal[2]);
	real cosI = dotI;

	real Rn = (n1 * cosI - n2 * cosR) / (n1 * cosI + n2 * cosR);
	real Rt = (n1 * cosR - n2 * cosI) / (n1 * cosR + n2 * cosI);
	
	// Reflectance = 1 - Trasmittance
	// Transmittance = (Rn^2+Rt^2) / 2
	return clamp01(1 - (Rn * Rn + Rt * Rt) / 2);
}

} // namespace proxo
