#include "../Light.h"
#include "Sun.h"
#include <algorithm>
#include "../math/Vector.h"
#include "../math/basics.h"

void SunLight::lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut) {
    real L[4], R[4], V[4];

    // L: vector point->transformedPosition
    substract(transformedPosition, point, L);
    real distToLight = std::max((real) 0, dot(L, direction));

    // intensity reduction with distance
    real lightIntensityInv = 1 + distToLight*distToLight * transformedReductionFactor;

    // dot product for diffuse intensity
    real D = std::max((real) 0, dot(direction, normal));

    // R: reflect of L with normal
    real N = 2 * dot(direction, normal);
    R[0] = N * normal[0] - direction[0];
    R[1] = N * normal[1] - direction[1];
    R[2] = N * normal[2] - direction[2];
    R[3] = 0;
    // V: normalized vector to camera
    normalize(point, V);
    // S: specular intensity
    real S = specularIntensity(dot(R, V), shininess) * specular;
    // C: diffuse intensity
    real C = diffuse * D;

    // final intensity
    real A = intensity / lightIntensityInv; // todo premult lightColor*intensity
    C *= A;
    S *= A;

    // Resulting color
    colorOut[0] += ((color[0] * C) + S) * this->color[0];
    colorOut[1] += ((color[1] * C) + S) * this->color[1];
    colorOut[2] += ((color[2] * C) + S) * this->color[2];
}
