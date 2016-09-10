#ifndef __SCENE_H__
#define __SCENE_H__

#include "Camera.h"
#include "Container.h"

class Scene {
public:
	Container world;
	Container body;
	Camera camera;
};

#endif
