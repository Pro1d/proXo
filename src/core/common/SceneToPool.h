#ifndef __SCENE_TO_POOL_H__
#define __SCENE_TO_POOL_H__

#include "core/model/Scene.h"
#include "core/common/Pool.h"
#include "core/model/TransformationStack.h"

class SceneToPool {
    public:
        SceneToPool();
        void run(Scene & scene, Pool & pool, bool skipNotVisibleObject);
        void objectToPool(Object & object, Material & material, Pool & pool, Camera & camera, bool skipNotVisibleObject);
        void lightToPool(Light & light, Pool & pool);
    private:
        TransformationStack transformation;
        Container * containerStack[50];
};

#endif // SCENETOPOOL_H
