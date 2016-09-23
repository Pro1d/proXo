#ifndef ENGINE_H
#define ENGINE_H

#include "Buffer.h"
#include "../model/Scene.h"
#include "../common/SceneToPool.h"
#include "../common/Pool.h"

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

    protected:

    private:
        Pool * pool;
        Scene * scene;
        Buffer * imageBuffer;
        SceneToPool sceneToPool;
};

#endif // ENGINE_H
