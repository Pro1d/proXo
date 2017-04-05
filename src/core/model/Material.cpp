#include "Material.h"
#include <limits>

namespace proxo {

Material::Material()
    : ambient(1), diffuse(1), specular(0.1), shininess(128), emissive(0),
      refractiveIndex(1), depthAbsorption{1,1,1}
{
}

} // namespace proxo
