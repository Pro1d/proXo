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

	real reflect; // vec3 rgb ?
	real refractiveIndex; // vec3 rgb ?
	real depthAbsorbtion;

protected:
private:
};

} // namespace proxo

#endif // MATERIAL_H
