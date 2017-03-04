#ifndef __SCENE_H__
#define __SCENE_H__

#include "Camera.h"
#include "Container.h"
#include "Material.h"
#include "Skybox.h"
#include <map>
#include <string>

class Scene {
public:
	Scene();
	~Scene();
	void print();
	void getCounts(positive& vertices, positive& faces, positive& lights);

	Container* world;
	Container* body;
	Camera camera;
	Skybox* skybox;

	std::map<std::string, Material*> materials;
	std::map<std::string, Object*> objects;
};

#endif
