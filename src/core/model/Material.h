#ifndef MATERIAL_H
#define MATERIAL_H

#include "core/math/type.h"

class Material
{
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

#endif // MATERIAL_H
