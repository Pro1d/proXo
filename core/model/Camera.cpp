#include "Camera.h"
#include "../math/type.h"
#include "../math/Matrix.h"

Camera::Camera() {
    identity(position);
    identity(projection);
    updateMatrix();
}

void Camera::setFrustrum(real zNear, real zFar, real width, real height) {
    projection[0] = 2 * zNear / width;
    projection[1] = 0;
    projection[2] = 0;
    projection[3] = 0;

    projection[4] = 0;
    projection[5] = 2 * zNear / height;
    projection[6] = 0;
    projection[7] = 0;

    projection[8] = 0;
    projection[9] = 0;
    projection[10] = -(zFar + zNear) / (zFar - zNear);
    projection[11] = -2*zFar*zNear / (zFar - zNear);

    projection[12] = 0;
    projection[13] = 0;
    projection[14] = -1;
    projection[15] = 0;

    this->zNear = zNear;
    this->zFar = zFar;
    xmax = width / 2;
    xmin = width / 2;
    ymax = height / 2;
    ymin = height / 2;

    updateMatrix();
}

void Camera::updateMatrix() {
    multiplyMM(position, projection, matrix);
}
