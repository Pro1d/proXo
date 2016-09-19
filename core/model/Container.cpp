#include <cstdio>
#include "Container.h"
#include "../math/type.h"
#include "../math/Matrix.h"
#include "Object.h"
#include "Light.h"


Container::Container(int maxObjectsCount, int maxContainersCount, int maxLightsCount) :
    objects(new Object*[maxObjectsCount]),
    objectMatrices(new mat4[maxObjectsCount]),
    objectMaterials(new Material*[maxObjectsCount]),
    containers(new Container*[maxContainersCount]),
    containerMatrices(new mat4[maxContainersCount]),
    lights(new Light*[maxLightsCount]),
    lightMatrices(new mat4[maxLightsCount]),
    objectsCount(0),
    containersCount(0),
    lightsCount(0)
{

}

Container::~Container() {
    for(positive i = 0; i < objectsCount; i++) {
        delete objectMatrices[i];
    }
    delete[] objects;
    delete[] objectMaterials;
    delete[] objectMatrices;

    for(positive i = 0; i < containersCount; i++) {
        delete containers[i];
        delete containerMatrices[i];
    }
    delete[] containers;
    delete[] containerMatrices;

    for(positive i = 0; i < lightsCount; i++) {
        delete lights[i];
        delete lightMatrices[i];
    }
    delete[] lights;
    delete[] lightMatrices;
}

void Container::addObject(Object * object, mat4 matrix, Material * material) {
    objectMatrices[objectsCount] = matrix;
    objectMaterials[objectsCount] = material;
    objects[objectsCount] = object;
    ++objectsCount;
}

void Container::addContainer(Container * container, mat4 matrix) {
    containerMatrices[containersCount] = matrix;
    containers[containersCount] = container;
    ++containersCount;
}

void Container::addLight(Light * light, mat4 matrix) {
    lightMatrices[lightsCount] = matrix;
    lights[lightsCount] = light;
    ++lightsCount;
}

void Container::print(positive depth) {
    for(positive i = 0; i < objectsCount; i++) {
        for(positive a = 0; a < depth; a++) printf("\t");
        printf("Object: %d vertices, %d faces\n", objects[i]->verticesCount, objects[i]->facesCount);
    }
    for(positive i = 0; i < lightsCount; i++) {
        for(positive a = 0; a < depth; a++) printf("\t");
        printf("Light\n");
    }
    for(positive i = 0; i < containersCount; i++) {
        containers[i]->print(depth+1);
    }
}

void Container::getCounts(positive & vertices, positive & faces, positive & lights) {
    vertices = 0;
    faces = 0;
    lights = 0;
    for(positive i = 0; i < objectsCount; i++) {
        vertices += objects[i]->verticesCount;
        faces += objects[i]->facesCount;
    }
    lights += lightsCount;
    for(positive i = 0; i < containersCount; i++) {
        positive f, v, l;
        containers[i]->getCounts(v, f, l);
        vertices += v;
        faces += f;
        lights += l;
    }
}
