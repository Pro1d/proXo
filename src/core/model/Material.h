#ifndef MATERIAL_H
#define MATERIAL_H

#include "core/math/type.h"

namespace proxo {

class Material {
public:
	Material();
	real ambient;
	real diffuse;
	real specular;
	real shininess;
	real emissive;

	real refractiveIndex; // vec3 rgb ?
	real depthAbsorption[VEC3_SCALARS_COUNT];

protected:
private:
};

} // namespace proxo

#endif // MATERIAL_H
