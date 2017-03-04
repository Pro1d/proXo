#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "Container.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "core/math/type.h"

class Container {
public:
	Container(int maxObjectsCount, int maxContainersCount, int maxLightsCount);
	~Container();
	void addObject(Object* object, mat4 matrix, Material* material);
	void addContainer(Container* container, mat4 matrix);
	void addLight(Light* light, mat4 matrix);

	void print(positive depth);
	void getCounts(positive& vertices, positive& faces, positive& lights);

	Object** objects;
	mat4* objectMatrices;
	Material** objectMaterials;

	Container** containers;
	mat4* containerMatrices;

	Light** lights;
	mat4* lightMatrices;

	positive objectsCount;
	positive containersCount;
	positive lightsCount;
};

#endif
