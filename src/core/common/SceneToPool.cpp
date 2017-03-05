#include "SceneToPool.h"
#include "core/math/Matrix.h"
#include "core/math/Vector.h"
#include "core/model/TransformationStack.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

SceneToPool::SceneToPool(MultiThread& mt) : transformation(50), multithread(mt)
{
}

void SceneToPool::run(Scene& scene, Pool& pool, bool skipNotVisibleObject)
{
	Container** contStackTop = containerStack;
	transformation.reset();

	// push body
	*contStackTop = scene.body;
	contStackTop++;
	transformation.save();

	// push world
	*contStackTop = scene.world;
	contStackTop++;
	transformation.saveAndPostMult(scene.camera.position);

	while(contStackTop != containerStack) {
		Container* top = *(--contStackTop);
		mat4 matrix    = transformation.getMatrix();

		if(top == NULL) {
			transformation.restore();
		}
		else {
			// Go trough object, light and containers
			for(integer i = top->objectsCount; --i >= 0;) {
				transformation.saveAndPostMult(top->objectMatrices[i]);
				objectToPool(*top->objects[i], *top->objectMaterials[i], pool,
				    scene.camera, skipNotVisibleObject);
				transformation.restore();
			}

			for(integer i = top->lightsCount; --i >= 0;) {
				transformation.saveAndPostMult(top->lightMatrices[i]);
				lightToPool(*top->lights[i], pool);
				transformation.restore();
			}

			*contStackTop = NULL;
			contStackTop++;
			for(integer i = top->containersCount; --i >= 0;) {
				// push container
				transformation.pushMult(matrix, top->containerMatrices[i]);
				*contStackTop = top->containers[i];
				contStackTop++;
			}
		}
	}
}

void SceneToPool::objectToPoolThread(
    void* data, positive threadId, positive threadsCount)
{
	ObjectToPoolInputs* inputs = (ObjectToPoolInputs*) data;
	Object& object             = *inputs->object;
	;
	Material& material    = *inputs->material;
	Pool& pool            = *inputs->pool;
	mat4 matrix           = inputs->transformationMatrix;
	positive vertexOffset = pool.currentVerticesCount;

	// Select range of data to process
	positive vertexIndexStart = threadId * object.verticesCount / threadsCount;
	positive vertexIndexEnd =
	    (threadId + 1) * object.verticesCount / threadsCount;
	positive faceIndexStart = threadId * object.facesCount / threadsCount;
	positive faceIndexEnd   = (threadId + 1) * object.facesCount / threadsCount;

	// Per vertex
	vec4 vertices = pool.vertexPool
	    + (vertexOffset + vertexIndexStart) * VEC4_SCALARS_COUNT;
	vec4 normals = pool.normalPool
	    + (vertexOffset + vertexIndexStart) * VEC4_SCALARS_COUNT;
	vec16 materials = pool.materialPool
	    + (vertexOffset + vertexIndexStart) * VEC16_SCALARS_COUNT;
	vec2 mappings = pool.mappingPool
	    + (vertexOffset + vertexIndexStart) * VEC2_SCALARS_COUNT;

	for(positive i = vertexIndexStart; i < vertexIndexEnd; i++) {
		// apply matrix to vertices and normals
		multiplyNoNormalizeMV(
		    matrix, object.vertices + i * VEC4_SCALARS_COUNT, vertices);
		multiplyNoNormalizeMV(
		    matrix, object.normals + i * VEC4_SCALARS_COUNT, normals);
		normalize(normals);

		vertices += VEC4_SCALARS_COUNT;
		normals += VEC4_SCALARS_COUNT;

		// copy colors and material
		if(object.colors != NULL)
			memcpy(materials, object.colors + i * VEC4_SCALARS_COUNT,
			    3 * sizeof(real));
		else {
			materials[0] = 1;
			materials[1] = 1;
			materials[2] = 1;
		}
		materials[MAT_POOL_INDEX_AMBIENT]    = material.ambient;
		materials[MAT_POOL_INDEX_DIFFUSE]    = material.diffuse;
		materials[MAT_POOL_INDEX_SPECULAR]   = material.specular;
		materials[MAT_POOL_INDEX_SHININESS]  = material.shininess;
		materials[MAT_POOL_INDEX_EMISSIVE]   = material.emissive;
		materials[MAT_POOL_INDEX_REFLECT]    = material.reflect;
		materials[MAT_POOL_INDEX_REFRACTIVE] = material.refractiveIndex;
		materials[MAT_POOL_INDEX_ABSORPTION] = material.depthAbsorbtion;
		materials += VEC16_SCALARS_COUNT;

		// copy colors, mappings and textures
		if(object.texture_mapping != NULL)
			memcpy(mappings, object.texture_mapping + i * VEC2_SCALARS_COUNT,
			    VEC2_SIZE);
		mappings += VEC2_SCALARS_COUNT;
	}

	// Per face
	positive* faces =
	    pool.facePool + (pool.currentFacesCount + faceIndexStart) * 4;

	positive* end = object.faces + (faceIndexEnd * 3);
	for(positive *f = object.faces + (faceIndexStart * 3); f != end;
	    f += 3, faces += 4) {
		// copy face's vertex indices and add offset
		faces[0] = f[0] + vertexOffset;
		faces[1] = f[1] + vertexOffset;
		faces[2] = f[2] + vertexOffset;
		faces[3] = object.texture_id;
	}
}

void SceneToPool::objectToPool(Object& object, Material& material, Pool& pool,
    Camera& camera, bool skipNotVisibleObject)
{
	// positive vertexOffset = pool.currentVerticesCount;
	mat4 matrix = transformation.getMatrix();

	if(skipNotVisibleObject) {
		// Do not add object to pool if object is not visible
		real center[VEC4_SCALARS_COUNT];
		multiplyMV(matrix, object.boundingSphereCenter, center);
		real radius = object.boundingSphereRadius * getMatrixScale(matrix);
		if(!camera.isShpereVisible(center, radius))
			return;
	}

	// inputs for objectToPoolThread
	ObjectToPoolInputs inputs = { matrix, &object, &material, &pool };

	// Run object to pool
	multithread.execute(SceneToPool::objectToPoolThread, (void*) &inputs);

	// Update pool sizes
	pool.currentVerticesCount += object.verticesCount;
	pool.currentFacesCount += object.facesCount;
}

void SceneToPool::lightToPool(Light& light, Pool& pool)
{
	light.transform(transformation.getMatrix());
	pool.lightPool[pool.currentLightsCount] = &light;
	pool.currentLightsCount++;
}
