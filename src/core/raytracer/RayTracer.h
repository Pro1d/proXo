#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include "DepthOfField.h"
#include "Intersection.h"
#include "KDTree.h"
#include "MaterialStack.h"
#include "core/common/Pool.h"
#include "core/common/SceneToPool.h"
#include "core/model/Scene.h"
#include "core/realtime/Buffer.h"
#include "core/sys/Multithreading.h"

namespace proxo {

class RayTracer {
public:
	RayTracer(Buffer* imageBuffer, Scene* scene);
	~RayTracer();
	void setScene(Scene* scene);

	void createMatchingPool();
	void render();
	static void threadRenderTask(
	    void* data, positive threadId, positive threadsCount);

	void getPointData(IntersectionData& intersect, Pool* pool, Material& mat,
	    vec4 point, vec4 normal, vec3 color, vec3 emissive);
	void getLightingColor(vec3 color, vec4 point, vec4 normal, Material& mat,
	    IntersectionData& intersect, TreeStack& treeStack,
	    MaterialStack& matStack, vec3 colorOut);
	void getRefractionColor(vec4 point, vec4 normal, vec3 dir, Material& mat,
	    IntersectionData& intersect, TreeStack& treeStack,
	    MaterialStack& matStack, real maxIntensity, vec3 colorOut,
	    real& refractRatioOut, positive depth);
	void getReflectionColor(vec4 point, vec4 normal, vec3 dir,
	    real refractRatio, Material& mat, IntersectionData& intersect,
	    TreeStack& treeStack, MaterialStack& matStack, real maxIntensity,
	    vec3 colorOut, positive depth);
	void getColor(vec3 orig, vec3 dir, real maxIntensity, positive* lastFace,
	    vec3 colorOut, real* depthOut, TreeStack& treeStack,
	    MaterialStack& matStack, positive depth = 0);

	Scene* scene;
	Pool* pool;
	KDTree* tree;
	Buffer* imageBuffer;
	MultiThread multithread;
	SceneToPool sceneToPool;

private:
	DepthOfField dof_;
	positive depthMax;
};

} // namespace proxo

#endif // __RAY_TRACER_H__
