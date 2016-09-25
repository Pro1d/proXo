#include <algorithm>
#include <cmath>
#include "Camera.h"
#include "../math/type.h"
#include "../math/Matrix.h"

Camera::Camera() {
    identity(position);
    identity(projection);
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
    xmin = -width / 2;
    ymax = height / 2;
    ymin = -height / 2;

    fieldOfView = atan2(width/2, zNear);
}

void Camera::setOrthographics(real zNear, real zFar, real width, real height) {
    projection[0] = 2 / width;
    projection[1] = 0;
    projection[2] = 0;
    projection[3] = 0;

    projection[4] = 0;
    projection[5] = 2 / height;
    projection[6] = 0;
    projection[7] = 0;

    projection[8] = 0;
    projection[9] = 0;
    projection[10] = -2 / (zFar - zNear);
    projection[11] = -(zFar + zNear) / (zFar - zNear);

    projection[12] = 0;
    projection[13] = 0;
    projection[14] = 0;
    projection[15] = 1;

    this->zNear = zNear;
    this->zFar = zFar;
    xmax = width / 2;
    xmin = -width / 2;
    ymax = height / 2;
    ymin = -height / 2;

    fieldOfView = 0;
}

void Camera::setScreenSize(real w, real h) {
    applyTranslate(projection, w/2, h/2, 0);

    projection[0] *= w/2;
    projection[5] *= -h/2;
}

void Camera::setFieldOfView(real fov) {
    fieldOfView = fov;
}

void Camera::lookAt(vec3 target) {
    // cartesian to polar coordiantes
    real d[3] = {
        -position[3] - target[0],
        -position[7] - target[1],
        -position[11] - target[2]
    };
    setDirection(d);
}

void Camera::setPosition(vec3 pos) {
    position[3] = -pos[0];
    position[7] = -pos[1];
    position[11] = -pos[2];
}

void Camera::setDirection(vec3 dir) {
    real x = dir[0];
    real y = dir[1];
    real z = dir[2];

    real d2 = x*x + y*y + z*z;
    if(d2 == 0) {
        // no scale/rotation
        for(positive i = 0; i < 3; i++)
        for(positive j = 0; j < 3; j++)
            position[i+j*4] = (i == j);
    }
    else {
        // R, Theta, Phi
        //real r = d2 * sqrt_inv(d2);
        real t = atan2(z, 1 / sqrt_inv(x*x+y*y));
        real p = atan2(y, x);
        real rotations[MAT4_SCALARS_COUNT];
        identity(rotations);

        applyRotate(rotations, -t, 0,1,0);
        applyRotate(rotations, -p, 0,0,1);
        real tmp[MAT4_SCALARS_COUNT];
        multiplyMM(position, rotations, tmp);
        copyMatrix(position, tmp);
    }
}

bool Camera::isShpereVisible(vec3 center, real radius) {
    if(fieldOfView == 0) {
        return center[0]+radius > xmin || center[0]-radius < xmax
            || center[1]+radius > ymin || center[1]-radius < ymax
            || center[2]+radius > zNear || center[2]-radius < zFar;
    }
    else {
        if(-center[2]+radius < zNear || -center[2]-radius > zFar)
            return false;

        // TODO optimize
        real lr = sqrt_inv(zNear*zNear + xmin*xmin);
        real left[3] = {xmin, 0, -zNear};
        real crossLeft = (left[2]*center[0]-center[2]*left[0])*lr;
        if(crossLeft > radius)
            return false;

        real right[3] = {xmax, 0, -zNear};
        real crossRight = (center[2]*right[0]-right[2]*center[0])*lr;
        if(crossRight > radius)
            return false;

        real bt = sqrt_inv(zNear*zNear + ymin*ymin);
        real bottom[3] = {0, ymin, -zNear};
        real crossBottom = (bottom[2]*center[1]-center[2]*bottom[1])*bt;
        if(crossBottom > radius)
            return false;

        real top[3] = {0, ymax, -zNear};
        real crossTop = (center[2]*top[1]-top[2]*center[1])*bt;
        if(crossTop > radius)
            return false;

        return true;
    }
}
