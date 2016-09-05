#ifndef __SCENE_TO_POOL_H__
#define __SCENE_TO_POOL_H__

class SceneToPool {
    public:
        SceneToPool();
        void run(Scene & scene, Pool & pool);
    private:
        Container * containerStack[50];
        TransformationStack transformation;
};

#endif // SCENETOPOOL_H
