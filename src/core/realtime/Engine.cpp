#include "Engine.h"
#include "Buffer.h"
#include "Rasterizer.h"
#include "core/common/Pool.h"
#include "core/common/SceneToPool.h"
#include "core/config/global.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include "core/model/Scene.h"
#include "core/sys/Multithreading.h"

Engine::Engine(Buffer* imageBuffer, Scene* scene)
    : pool(NULL), scene(scene), imageBuffer(imageBuffer), multithread(THREADS_COUNT),
      sceneToPool(multithread)
{
	// ctor
}

Engine::~Engine()
{
	if(pool != NULL)
		delete pool;
}

void Engine::setScene(Scene* scene)
{
	this->scene = scene;
}

void Engine::createMatchingPool()
{
	if(pool != NULL)
		delete pool;
	positive vertices, faces, lights;
	scene->getCounts(vertices, faces, lights);
	pool = new Pool(vertices, faces, lights);
}

void clearBufferThread(void* data, positive threadId, positive threadsCount)
{
	Engine* that = (Engine*) data;
	that->imageBuffer->clear(threadId * that->imageBuffer->height / threadsCount,
	    (threadId + 1) * that->imageBuffer->height / threadsCount);
}

void Engine::render()
{
	pool->reset();

	sceneToPool.run(*scene, *pool, true);

	multithread.execute(clearBufferThread, (void*) this);

	vertexLighting();

	drawTriangles();
}

void vertexLigthingThread(void* data, positive threadId, positive threadsCount)
{
	Engine* that = (Engine*) data;
	real v[VEC4_SCALARS_COUNT];
	vec4 vertex    = that->pool->vertexPool + VEC4_SCALARS_COUNT * threadId;
	vec16 material = that->pool->materialPool + VEC16_SCALARS_COUNT * threadId;

	for(positive i = threadId; i < that->pool->currentVerticesCount; i += threadsCount) {

		// Emissive light
		real emissive                  = material[MAT_POOL_INDEX_EMISSIVE];
		real color[VEC3_SCALARS_COUNT] = { material[0] * emissive, material[1] * emissive,
			material[2] * emissive };

		// Add lights
		for(positive j = 0; j < that->pool->currentLightsCount; j++)
			that->pool->lightPool[j]->lighting(material,
			    that->pool->normalPool + i * VEC4_SCALARS_COUNT, vertex,
			    material[MAT_POOL_INDEX_AMBIENT], material[MAT_POOL_INDEX_DIFFUSE],
			    material[MAT_POOL_INDEX_SPECULAR], material[MAT_POOL_INDEX_SHININESS], color);

		// Save resulting color
		material[0] = color[0];
		material[1] = color[1];
		material[2] = color[2];

		multiplyMV(that->scene->camera.projection, vertex, v);
		memcpy(vertex, v, VEC4_SIZE);

		vertex += VEC4_SCALARS_COUNT * threadsCount;
		material += VEC16_SCALARS_COUNT * threadsCount;
	}
}

void Engine::vertexLighting()
{
	multithread.execute(vertexLigthingThread, (void*) this);
}

void drawTriangleThread(void* data, positive threadId, positive threadsCount)
{
	Engine* that = (Engine*) data;
	integer i    = that->pool->currentFacesCount + threadId;
	while((i -= threadsCount) >= 0) {
		positive* face = that->pool->facePool + i * 4;
		vec4 v1        = that->pool->vertexPool + face[0] * VEC4_SCALARS_COUNT;
		vec4 v2        = that->pool->vertexPool + face[1] * VEC4_SCALARS_COUNT;
		vec4 v3        = that->pool->vertexPool + face[2] * VEC4_SCALARS_COUNT;

		// Back-face culling
		if(isFaceOrientationZPositive(v1, v2, v3))
			continue;
		if((v1[2] < 0 && v2[2] < 0 && v3[2] < 0) || (v1[2] > 1 && v2[2] > 1 && v3[2] > 1))
			continue;

		vec3 c1 = that->pool->materialPool + face[0] * VEC16_SCALARS_COUNT;
		vec3 c2 = that->pool->materialPool + face[1] * VEC16_SCALARS_COUNT;
		vec3 c3 = that->pool->materialPool + face[2] * VEC16_SCALARS_COUNT;

		/** Normal for debug * /
		real c1[3] = {
		    n1[0]*0.5+0.5,
		    n1[1]*0.5+0.5,
		    n1[2]*0.5+0.5
		};
		real c2[3] = {
		    n2[0]*0.5+0.5,
		    n2[1]*0.5+0.5,
		    n2[2]*0.5+0.5
		};
		real c3[3] = {
		    n3[0]*0.5+0.5,
		    n3[1]*0.5+0.5,
		    n3[2]*0.5+0.5
		};*/

		/** Deth for debug * /
		real c1[3] = {
		    v1[2],
		    v1[2],
		    v1[2]
		};
		real c2[3] = {
		    v2[2],
		    v2[2],
		    v2[2]
		};
		real c3[3] = {
		    v3[2],
		    v3[2],
		    v3[2]
		};*/

		triangle(*that->imageBuffer, v1, v2, v3, c1, c2, c3);
	}
}

void Engine::drawTriangles()
{
	multithread.execute(drawTriangleThread, (void*) this);
}
