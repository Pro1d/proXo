#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "core/math/type.h"

class Camera {
public:
    Camera();

    void setFieldOfView(real fov);
    real getFieldOfView();
    void setDepthMax(real zFar);
    void setFrustrum(real zNear, real zFar, real width, real height);
    void setOrthographics(real zNear, real zFar, real width, real height);
    void setScreenSize(real w, real h);
    void updateProjection();

    void lookAt(vec3 target);
    void setPosition(vec3 pos);
    void setDirection(vec3 dir);

    bool isShpereVisible(vec3 center, real radius);

    real projection[MAT4_SCALARS_COUNT];
    real position[MAT4_SCALARS_COUNT];

    real fieldOfView;
	real zNear, zFar;
	real xmin, xmax;
	real ymin, ymax;
	real screenWidth, screenHeight;
};

#endif
