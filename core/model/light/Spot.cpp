#include "../Light.h"
#include "Spot.h"
#include <algorithm>
#include "../math/Vector.h"
#include "../math/basics.h"


void SpotLight::lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut) {
    real L[4], R[4], V[4];

    // L: vector point->transformedPosition
    substract(transformedPosition, point, L);
    real distToLight2 = squaredLength(L);
    real distToLightInv = sqrt_inv(distToLight2);
    // intensity reduction with distance
    real lightIntensityInv = 1 + distToLight2*transformedReductionFactor;

    // normalize L
    multiply(L, distToLightInv);
    // dot product for diffuse intensity
    real D = std::max((real) 0, dot(L, normal));

    // Intensity from cut off/fall off
    real I = -dot(direction, L);
    if(I < cutOff) {
        // outside
        return;
    }
    else if(I < fallOff) {
        // bound: interpolate
        I = (cutOff-I) / (cutOff-fallOff);
    }
    else {
        // center: full intesity
        I = 1;
    }

    // R: reflect of L with normal
    real N = 2 * dot(L, normal);
    R[0] = N * normal[0] - L[0];
    R[1] = N * normal[1] - L[1];
    R[2] = N * normal[2] - L[2];
    R[3] = 0;
    // V: normalized vector to camera
    normalize(point, V);

    // S: specular intensity
    real S = specularIntensity(dot(R, V), shininess) * specular;
    // C: diffuse intensity
    real C = diffuse * D;

    // final intensity
    real A = I * intensity / lightIntensityInv; // todo premult lightColor*intensity
    C *= A;
    S *= A;

    // Resulting color
    colorOut[0] += ((color[0] * C) + S) * this->color[0];
    colorOut[1] += ((color[1] * C) + S) * this->color[1];
    colorOut[2] += ((color[2] * C) + S) * this->color[2];
}
