#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../math/type.h"

class Camera {
public:
    Camera();
    void setFrustrum(real zNear, real zFar, real width, real height);
    void lookAt(vec3 target);
    void setPosition(vec3 pos);
    void setDirection(vec3 dir);
    void setFieldOfView(real fov);

	real matrix[MAT4_SCALARS_COUNT];
private:
    void updateMatrix();

    real fieldOfView;
    real projection[MAT4_SCALARS_COUNT];
    real position[MAT4_SCALARS_COUNT];
	real zNear, zFar;
	real xmin, xmax;
	real ymin, ymax;
};

#endif
