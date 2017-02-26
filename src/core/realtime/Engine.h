#ifndef ENGINE_H
#define ENGINE_H

#include "Buffer.h"
#include "core/model/Scene.h"
#include "core/common/SceneToPool.h"
#include "core/common/Pool.h"
#include "core/sys/Multithreading.h"

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
        MultiThread multithread;
        SceneToPool sceneToPool;
};

#endif // ENGINE_H
