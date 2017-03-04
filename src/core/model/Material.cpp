#include "Material.h"
#include <limits>

Material::Material()
    : ambient(1), diffuse(1), specular(0.5), shininess(128), emissive(0), reflect(0),
      refractiveIndex(1), depthAbsorbtion(1)
{
}
