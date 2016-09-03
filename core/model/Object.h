#ifndef __OBJECT_H__
#define __OBJECT_H__

class Object {
	public:
		Object();
		~Object();
	
		vec4 vertices;
		vec4 normals;
		vec4 colors;
		vec2 texture_mapping;
		positive * faces;
		positive texture_id;
		positive verticesCount;
		positive facesCount;
};

#endif