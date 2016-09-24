#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "../math/type.h"

class Object {
	public:
		Object();
		~Object();
        void initialize();
		void allocateVertices();
        void allocateNormals();
        void allocateColors();
        void allocateTextureMapping();
        void allocateFaces(positive count);
        void updateBoundingSphere();

		vec4 vertices;
		vec4 normals;
		vec4 colors;
		vec2 texture_mapping;
		positive * faces;
		positive texture_id;
		positive verticesCount;
		positive facesCount;
        real boundingSphereCenter[VEC4_SCALARS_COUNT];
        real boundingSphereRadius;
};

#endif
