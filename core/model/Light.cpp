#include <algorithm>
#include "Light.h"
#include "../math/type.h"
#include "../math/Matrix.h"


Light::~Light() {

}

void Light::initialize() {
    color[0] = 1;
    color[1] = 1;
    color[2] = 1;
    intensity = 1;
    reductionFactor = 0;
    direction[0] = 1;
    direction[1] = 0;
    direction[2] = 0;
    direction[3] = 0;
    position[0] = 0;
    position[1] = 0;
    position[2] = 0;
    position[3] = 1;
}

void Light::setDirection(vec3 dir) {
    direction[0] = dir[0];
    direction[1] = dir[1];
    direction[2] = dir[2];
}
void Light::setPosition(vec3 pos) {
    position[0] = pos[0];
    position[1] = pos[1];
    position[2] = pos[2];
}
void Light::setColor(vec3 c) {
    color[0] = c[0];
    color[1] = c[1];
    color[2] = c[2];
}

void Light::setIntensity(real i) {
    intensity = i;
}
void Light::setReductionFactor(real i) {
    reductionFactor = i;
}
void Light::setFallOff(real i) {
    fallOff = cos(toRadians(i));
}
void Light::setCutOff(real i) {
    cutOff = cos(toRadians(i));
}

void Light::transform(mat4 matrix) {
    multiplyMV(matrix, position, transformedPosition);
    multiplyMV(matrix, direction, transformedDirection);
    transformedReductionFactor *= getMatrixScale(matrix);
}

real Light::specularIntensity(real RdotV, real shininess) {
    real k = (1-shininess*(1-RdotV)/8);
    k *= k;
    k *= k;
    k *= k;
    return std::max((real) 0, k);
}
