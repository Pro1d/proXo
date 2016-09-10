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
