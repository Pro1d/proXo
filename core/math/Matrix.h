#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "type.h"
#include <cstdlib>
#include <cmath>
#include "Matrix.h"
#include "type.h"
#include "basics.h"

mat4 newMatrix();
void identity(mat4 mOut);

void multiplyMM(mat4 A, mat4 B, mat4 mOut);
void multiplyMV(mat4 M, vec4 v, vec4 vOut);
void multiplyNoNormalizeMV(mat4 M, vec4 v, vec4 vOut);

void copyMatrix(mat4 dst, const mat4 src);

void applyRotate(mat4 mOut, real angle, real x, real y, real z);
void setRotateX(mat4 mOut, real angle);
void setRotateY(mat4 mOut, real angle);
void setRotateZ(mat4 mOut, real angle);
void setRotate(mat4 mOut, real angle, real x, real y, real z);

void applyTranslate(mat4, real x, real y, real z);
void setTranslate(mat4 mOut, real x, real y, real z);

void applyScale(mat4 mOut, real s);
void setScale(mat4 mOut, real x, real y, real z);
real getMatrixScale(mat4 m);


#endif
