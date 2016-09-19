#include <cstdlib>
#include "Object.h"
#include "../math/type.h"

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
}
