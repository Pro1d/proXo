#include "Matrix.h"
#include "type.h"
#include <cstdlib>

namespace proxo {

void fitWithScreenCoordinates(real screenWidth, real screenHeight, mat4 mOut)
{
	real half_w = screenWidth / 2;
	real half_h = screenHeight / 2;
	mOut[0]     = half_w;
	mOut[1]     = 0;
	mOut[2]     = 0;
	mOut[3]     = half_w;
	mOut[4]     = 0;
	mOut[5]     = -half_h;
	mOut[6]     = 0;
	mOut[7]     = -half_h;
	mOut[8]     = 0;
	mOut[9]     = 0;
	mOut[10]    = 1;
	mOut[11]    = 0;
	mOut[12]    = 0;
	mOut[13]    = 0;
	mOut[14]    = 0;
	mOut[15]    = 1;
}

} // namespace proxo

/*
void Camera::setOrthographic(real zNear, real zFar, real width, real height) {
    this->zNear = zNear;
    this->zFar = zFar;
    xmax = width / 2;
    xmin = width / 2;
    ymax = height / 2;
    ymin = height / 2;

    real scale = std::max(width, height);
    projection[0] = 2 / scale;
    projection[1] = 0;
    projection[2] = 0;
    projection[3] = 0;

    projection[4] = 0;
    projection[5] = 2 / scale;
    projection[6] = 0;
    projection[7] = 0;

    projection[8] = 0;
    projection[9] = 0;
    projection[10] = -2 / (zFar-zNear);
    projection[11] = -(zFar+zNear) / (zFar-zNear);

    projection[12] = 0;
    projection[13] = 0;
    projection[14] = 0;
    projection[15] = 1;

    updateMatrix();
}*/

/*
void Camera::setOrthographic(real zNear, real zFar, real width, real height) {
    this->zNear = zNear;
    this->zFar = zFar;
    xmax = width / 2;
    xmin = width / 2;
    ymax = height / 2;
    ymin = height / 2;

    real scale = std::max(width, height);
    projection[0] = 2 / scale;
    projection[1] = 0;
    projection[2] = 0;
    projection[3] = 0;

    projection[4] = 0;
    projection[5] = 2 / scale;
    projection[6] = 0;
    projection[7] = 0;

    projection[8] = 0;
    projection[9] = 0;
    projection[10] = -2 / (zFar-zNear);
    projection[11] = -(zFar+zNear) / (zFar-zNear);

    projection[12] = 0;
    projection[13] = 0;
    projection[14] = 0;
    projection[15] = 1;

    updateMatrix();
}*/
