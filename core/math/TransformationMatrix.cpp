#include <cstdib>
#include "Matrix.h"

void fitWithScreenCoordinates(real screenWidth, real screenHeight, mat4 mOut) {
	real half_w = screenWidth / 2;
	real half_h = screenHeight / 2;
	mOut[0] =  half_w; mOut[1] =  0; mOut[2] =  0; mOut[3] =  half_w;
	mOut[4] =  0; mOut[5] =  -half_h; mOut[6] =  0; mOut[7] =  -half_h;
	mOut[8] =  0; mOut[9] =  0; mOut[10] = 1; mOut[11] = 0;
	mOut[12] = 0; mOut[13] = 0; mOut[14] = 0; mOut[15] = 1;
}
