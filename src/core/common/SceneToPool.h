#ifndef __SCENE_TO_POOL_H__
#define __SCENE_TO_POOL_H__

#include "core/common/Pool.h"
#include "core/model/Scene.h"
#include "core/model/TransformationStack.h"
#include "core/sys/Multithreading.h"

namespace proxo {

typedef struct {
	mat4 transformationMatrix;
	Object* object;
	Material* material;
	Pool* pool;
	positive texture_id;
} ObjectToPoolInputs;

class SceneToPool {
public:
	SceneToPool(MultiThread& mt);
	void run(Scene& scene, Pool& pool, bool skipNotVisibleObject);
	void objectToPool(Object& object, Material& material, Pool& pool,
	    Camera& camera, bool skipNotVisibleObject);
	void lightToPool(Light& light, Pool& pool);
	static void objectToPoolThread(
	    void* data, positive threadId, positive threadsCount);

private:
	TransformationStack transformation;
	Container* containerStack[50];
	MultiThread& multithread;
};

} // namespace proxo

#endif // SCENETOPOOL_H
