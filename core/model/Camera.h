#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../math/type.h"

class Camera {
public:
    Camera();
    void setFrustrum(real zNear, real zFar, real width, real height);

	real matrix[MAT4_SCALARS_COUNT];
private:
    void updateMatrix();

    real projection[MAT4_SCALARS_COUNT];
    real position[MAT4_SCALARS_COUNT];
	real zNear, zFar;
	real xmin, xmax;
	real ymin, ymax;
};

#endif
