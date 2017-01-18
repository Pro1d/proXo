#ifndef ENGINE_H
#define ENGINE_H

#include "Buffer.h"
#include "../model/Scene.h"
#include "../common/SceneToPool.h"
#include "../common/Pool.h"
#include "../sys/Multithreading.h"

class Engine
{
    public:
        Engine(Buffer * imageBuffer, Scene * scene);
        ~Engine();
        void setScene(Scene * scene);
        void createMatchingPool();
        void render();
        void vertexLighting();
        void drawTriangles();

        Pool * pool;
        Scene * scene;
        Buffer * imageBuffer;
        SceneToPool sceneToPool;
        MultiThread multithread;
};

#endif // ENGINE_H
