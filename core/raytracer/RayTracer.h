#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include "../model/Scene.h"
#include "../common/Pool.h"
#include "../common/SceneToPool.h"
#include "../sys/Multithreading.h"
#include "../realtime/Buffer.h"
#include "KDTree.h"

class RayTracer {
public:
    RayTracer(Buffer * imageBuffer, Scene * scene);
    ~RayTracer();
    void setScene(Scene * scene);
    void createMatchingPool();
    void render();
    positive getColor(vec3 orig, vec3 dir, real currentRefractiveIndex, vec3 colorOut, real * depthOut, TreeStack & stack);

    Scene * scene;
    Pool * pool;
    KDTree * tree;
    Buffer * imageBuffer;
    SceneToPool sceneToPool;
    MultiThread multithread;
};

#endif // __ENGINE_H__
