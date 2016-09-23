#ifndef __SCENE_TO_POOL_H__
#define __SCENE_TO_POOL_H__

#include "../model/Scene.h"
#include "../common/Pool.h"
#include "../model/TransformationStack.h"

class SceneToPool {
    public:
        SceneToPool();
        void run(Scene & scene, Pool & pool);
        void objectToPool(Object & object, Material & material, Pool & pool);
        void lightToPool(Light & light, Pool & pool);
    private:
        TransformationStack transformation;
        Container * containerStack[50];
};

#endif // SCENETOPOOL_H
