#include "Scene.h"
#include <map>
#include <string>
#include <cstdio>
#include "Material.h"
#include "Object.h"

Scene::Scene() : world(NULL), body(NULL), skybox(NULL) {
    materials["default"] = new Material;
}

Scene::~Scene() {
    if(skybox != NULL)
        delete[] skybox;
    for (std::map<std::string, Material*>::iterator it = materials.begin(); it != materials.end(); ++it)
        delete it->second;
    for (std::map<std::string, Object*>::iterator it = objects.begin(); it != objects.end(); ++it)
        delete it->second;
    if(world != NULL)
        delete world;
    if(body != NULL)
        delete body;
}

void Scene::print() {
    printf("Camera\n");
    printf("Skybox: %p\n", skybox);

    printf("World\n");
    world->print(1);
    printf("Body\n");
    body->print(1);

    positive vertices, faces, lights;
    getCounts(vertices, faces, lights);
    printf("Total vertices count: %d\n", vertices);
    printf("Total faces count: %d\n", faces);
    printf("Total lights count: %d\n", lights);
}
void Scene::getCounts(positive & vertices, positive & faces, positive & lights) {
    positive v,f,l;
    vertices = 0;
    faces = 0;
    lights = 0;

    world->getCounts(v, f, l);
    vertices += v;
    faces += f;
    lights += l;

    body->getCounts(v, f, l);
    vertices += v;
    faces += f;
    lights += l;
}
