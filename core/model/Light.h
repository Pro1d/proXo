#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../math/type.h"

class Light {
public:
    Light();
    virtual ~Light();
    void initialize();
    void transform(mat4 matrix);
    virtual void lighting(vec4 color, vec4 normal, vec4 point, real ambient, real diffuse, real specular, real shininess, vec4 colorOut) = 0;
    void setDirection(vec3 dir);
    void setPosition(vec3 pos);
    void setColor(vec3 c);
    void setIntensity(real i);
    void setReductionFactor(real i);
    void setFallOff(real i);
    void setCutOff(real i);
protected:
    real specularIntensity(real RdotV, real shininess);

    real transformedPosition[VEC4_SCALARS_COUNT];
    real transformedDirection[VEC4_SCALARS_COUNT];
    real transformedReductionFactor;
    real color[VEC4_SCALARS_COUNT];
    real intensity;
    real reductionFactor;//  1 / (d^2 * factor + 1);
    real fallOff, cutOff;
    real direction[VEC4_SCALARS_COUNT];
    real position[VEC4_SCALARS_COUNT];
};

#endif // __LIGHT_H__
