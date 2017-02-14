#include <cstdlib>
#include <algorithm>
#include "Object.h"
#include "core/math/type.h"
#include "core/math/Vector.h"

Object::Object(/*const char * filename, positive texture_id*/) :
		vertices(NULL),
		normals(NULL),
		colors(NULL),
		texture_mapping(NULL),
		faces(NULL),
		texture_id(0),
		verticesCount(0),
		facesCount(0)
{

}

void Object::initialize() {
    vertices = (NULL);
    normals = (NULL);
    colors = (NULL);
    texture_mapping = (NULL);
    faces = (NULL);
    texture_id = (0);
    verticesCount = (0);
    facesCount = (0);
}

Object::~Object() {
	if(vertices != NULL)
		delete[] vertices;
	if(normals != NULL)
		delete[] normals;
	if(colors != NULL)
		delete[] colors;
	if(texture_mapping != NULL)
		delete[] texture_mapping;
	if(faces != NULL)
		delete[] faces;
}

void Object::allocateVertices() {
    vertices = new real[verticesCount * VEC4_SCALARS_COUNT];
}

void Object::allocateNormals() {
    normals = new real[verticesCount * VEC4_SCALARS_COUNT];
}

void Object::allocateColors() {
    colors = new real[verticesCount * VEC4_SCALARS_COUNT];
}

void Object::allocateTextureMapping() {
    texture_mapping = new real[verticesCount * VEC2_SCALARS_COUNT];
}

void Object::allocateFaces(positive count) {
    faces = new positive[count * 3];
    faceNormals = new real[count * VEC4_SCALARS_COUNT];
}

void Object::updateBoundingSphere() {
    real rectMax[VEC4_SCALARS_COUNT], rectMin[VEC4_SCALARS_COUNT];

    rectMax[0] = rectMin[0] = vertices[0];
    rectMax[1] = rectMin[1] = vertices[1];
    rectMax[2] = rectMin[2] = vertices[2];

    for(positive i = 1; i < verticesCount; i++) {
        rectMax[0] = std::max(rectMax[0], vertices[i*VEC4_SCALARS_COUNT+0]);
        rectMax[1] = std::max(rectMax[1], vertices[i*VEC4_SCALARS_COUNT+1]);
        rectMax[2] = std::max(rectMax[2], vertices[i*VEC4_SCALARS_COUNT+2]);
        rectMin[0] = std::min(rectMin[0], vertices[i*VEC4_SCALARS_COUNT+0]);
        rectMin[1] = std::min(rectMin[1], vertices[i*VEC4_SCALARS_COUNT+1]);
        rectMin[2] = std::min(rectMin[2], vertices[i*VEC4_SCALARS_COUNT+2]);
    }

    boundingSphereCenter[0] = (rectMin[0]+rectMax[0]) / 2;
    boundingSphereCenter[1] = (rectMin[1]+rectMax[1]) / 2;
    boundingSphereCenter[2] = (rectMin[2]+rectMax[2]) / 2;
    boundingSphereCenter[3] = 1;

    real radiusSquaredMax = 0;
    for(positive i = 1; i < verticesCount; i++) {
        real v[VEC4_SCALARS_COUNT];
        substract(boundingSphereCenter, vertices+i*VEC4_SCALARS_COUNT, v);
        radiusSquaredMax = std::max(radiusSquaredMax, squaredLength(v));
    }

    boundingSphereRadius = radiusSquaredMax * sqrt_inv(radiusSquaredMax);

    /** Other possible method: Ritter's bounding sphere algorithm
        1. Pick a point x from P, search a point y in P, which has the largest distance from x;
        2. Search a point z in P, which has the largest distance from y. set up an initial ball B,
        with its centre as the midpoint of y and z, the radius as half of the distance between y and z;
        3. If all points in P are within ball B, then we get a bounding sphere. Otherwise,
        let p be a point outside the ball, construct a new ball covering both point p and previous ball.
        Repeat this step until all points are covered.
    */
}
