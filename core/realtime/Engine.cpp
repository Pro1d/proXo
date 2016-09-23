#include "Engine.h"
#include "Buffer.h"
#include "Rasterizer.h"
#include "../model/Scene.h"
#include "../common/SceneToPool.h"
#include "../common/Pool.h"
#include "../math/type.h"

Engine::Engine(Buffer * imageBuffer, Scene * scene) :
    pool(NULL), scene(scene), imageBuffer(imageBuffer)
{
    //ctor
}

Engine::~Engine()
{
    if(pool != NULL)
        delete pool;
}

void Engine::setScene(Scene * scene)
{
    this->scene = scene;
}

void Engine::createMatchingPool()
{
    if(pool != NULL)
        delete pool;
    positive vertices, faces, lights;
    scene->getCounts(vertices, faces, lights);
    pool = new Pool(vertices, faces, lights);
}
#include <cstdio>
void Engine::render() {
    pool->reset();

    sceneToPool.run(*scene, *pool);

    imageBuffer->clear();

    vertexLighting();

    drawTriangles();
}

void Engine::vertexLighting() {
    real v[VEC4_SCALARS_COUNT];
    vec4 vertex = pool->vertexPool;
    vec4 material = pool->materialPool;

    for(positive i = 0; i < pool->currentVerticesCount; i++) {

        // Emissive light
        real emissive = material[7];
        real color[VEC3_SCALARS_COUNT] = {
            material[0]*emissive,
            material[1]*emissive,
            material[2]*emissive
        };

        // Add lights
        for(positive j = 0; j < pool->currentLightsCount; j++)
            pool->lightPool[j]->lighting(material, pool->normalPool+i*VEC4_SCALARS_COUNT, vertex,
                                         material[3], material[4], material[5], material[6], color);

        // Save resulting color
        material[0] = color[0];
        material[1] = color[1];
        material[2] = color[2];

        multiplyMV(scene->camera.projection, vertex, v);
        memcpy(vertex, v, VEC4_SIZE);

        vertex += VEC4_SCALARS_COUNT;
        material += VEC8_SCALARS_COUNT;
    }

}

void Engine::drawTriangles() {
    integer i = pool->currentFacesCount;
    while(--i >= 0) {
        positive * face = pool->facePool + i * 3;
        vec4 v1 = pool->vertexPool + face[0]*VEC4_SCALARS_COUNT;
        vec4 v2 = pool->vertexPool + face[1]*VEC4_SCALARS_COUNT;
        vec4 v3 = pool->vertexPool + face[2]*VEC4_SCALARS_COUNT;

        vec3 c1 = pool->materialPool + face[0]*VEC8_SCALARS_COUNT;
        vec3 c2 = pool->materialPool + face[1]*VEC8_SCALARS_COUNT;
        vec3 c3 = pool->materialPool + face[2]*VEC8_SCALARS_COUNT;

        /** Normal for debug * /
        real c1[3] = {
            n1[0]*0.5+0.5,
            n1[1]*0.5+0.5,
            n1[2]*0.5+0.5
        };
        real c2[3] = {
            n2[0]*0.5+0.5,
            n2[1]*0.5+0.5,
            n2[2]*0.5+0.5
        };
        real c3[3] = {
            n3[0]*0.5+0.5,
            n3[1]*0.5+0.5,
            n3[2]*0.5+0.5
        };*/

        triangle(*imageBuffer, v1, v2, v3, c1, c2, c3);
    }
}
