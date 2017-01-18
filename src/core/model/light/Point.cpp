#include <algorithm>
#include "Point.h"
#include "../math/Vector.h"
#include "../math/basics.h"

PointLight::PointLight() : Light(true) {
}

void PointLight::lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut) {
    real L[4], R[4], V[4];

    // L: vector point->transformedPosition
    substract(transformedPosition, point, L);
    real distToLight2 = squaredLength(L);

    // light is too far, attenuation makes it invisible
    if(transformedReductionFactor > 0 && distToLight2 > transformedDistanceMax)
        return;

    real distToLightInv = sqrt_inv(distToLight2);
    // intensity reduction with distance
    real lightIntensityInv = 1 + distToLight2*transformedReductionFactor;

    // normalize L
    multiply(L, distToLightInv);
    real LdotNormal = dot(L, normal);
    // dot product for diffuse intensity
    real D = std::max((real) 0, LdotNormal);

    // R: reflect of L with normal
    real N = 2 * LdotNormal;
    R[0] = N * normal[0] - L[0];
    R[1] = N * normal[1] - L[1];
    R[2] = N * normal[2] - L[2];
    R[3] = 0;
    // V: normalized vector to camera
    normalize(point, V);
    // S: specular intensity
    real S = specularIntensity(-dot(R, V), shininess) * specular;
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

real PointLight::getDirectionToSource(vec4 point, vec4 directionOut) {
    substract(transformedPosition, point, directionOut);
    real norm2 = squaredLength(directionOut);
    real norm_inv = sqrt_inv(norm2);
    multiply(directionOut, norm_inv);
    return norm_inv * norm2;
}
