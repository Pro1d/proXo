#include <algorithm>
#include "Light.h"
#include "../math/type.h"
#include "../math/Matrix.h"

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
