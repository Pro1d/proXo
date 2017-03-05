#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include "KDTree.h"
#include "core/common/Pool.h"
#include "core/common/SceneToPool.h"
#include "core/model/Scene.h"
#include "core/realtime/Buffer.h"
#include "core/sys/Multithreading.h"

class RayTracer {
public:
	RayTracer(Buffer* imageBuffer, Scene* scene);
	~RayTracer();
	void setScene(Scene* scene);
	void createMatchingPool();
	void render();
	positive getColor(vec3 orig, vec3 dir, real currentRefractiveIndex,
	    real maxIntensity, positive* lastFace, vec3 colorOut, real* depthOut,
	    TreeStack& stack);

	Scene* scene;
	Pool* pool;
	KDTree* tree;
	Buffer* imageBuffer;
	MultiThread multithread;
	SceneToPool sceneToPool;
};

#endif // __ENGINE_H__
