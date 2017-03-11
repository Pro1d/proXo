#include "RayTracer.h"
#include "Intersection.h"
#include "Physic.h"
#include "core/math/Vector.h"
#include "core/math/type.h"

namespace proxo {

#define EPSILON ((real) 1 / 255)

RayTracer::RayTracer(Buffer* imageBuffer, Scene* scene)
    : scene(scene), pool(NULL), tree(NULL), imageBuffer(imageBuffer),
      multithread(THREADS_COUNT), sceneToPool(multithread)
{
}

RayTracer::~RayTracer()
{
	if(pool != NULL)
		delete pool;
	if(tree != NULL)
		delete tree;
}

void RayTracer::setScene(Scene* scene)
{
	this->scene = scene;
}

void RayTracer::createMatchingPool()
{
	if(pool != NULL)
		delete pool;
	positive vertices, faces, lights;
	scene->getCounts(vertices, faces, lights);
	pool = new Pool(vertices, faces, lights);
}

void threadRenderTask(void* data, positive threadId, positive threadsCount)
{
	RayTracer* that = (RayTracer*) data;
	TreeStack stack(512);

	real rayOrig[VEC4_SCALARS_COUNT] = { 0, 0, 0, 1 };
	real rayDir[VEC4_SCALARS_COUNT]  = { 0, 0, 0, 0 };

	Camera& cam(that->scene->camera);
	real xmin = cam.xmin;
	real dx   = cam.xmax - xmin;
	real ymin = cam.ymin;
	real dy   = cam.ymax - ymin;

	for(positive y = threadId; y < that->imageBuffer->height;
	    y += threadsCount) {
		for(positive x = 0; x < that->imageBuffer->width; x++) {
			rayDir[0] = ((real) x / cam.screenWidth) * dx + xmin;
			rayDir[1] = (1 - (real) y / cam.screenHeight) * dy + ymin;
			rayDir[2] = -cam.zNear;
			normalize(rayDir);

			vec4 pxl = that->imageBuffer->getPtr(x, y);

			that->getColor(rayOrig, rayDir, 1, 1, NULL, pxl + 1, pxl, stack);
		}
	}
}
#include <cstdio>
positive RayTracer::getColor(vec3 orig, vec3 dir, real currentRefractiveIndex,
    real maxIntensity, positive* lastFace, vec3 colorOut, real* depthOut,
    TreeStack& stack)
{
	colorOut[0] = 0;
	colorOut[1] = 0;
	colorOut[2] = 0;

	if(maxIntensity < (real) 1 / 255)
		return 0;

	IntersectionData intersect;
	intersectTree(
	    orig, dir, tree, stack, pool->vertexPool, lastFace, intersect);

	/// TODO translucent, couleur perçu absorbée par épaisseur d'objet pas
	/// complètement transparent
	/// -> color when go out of object, 1-(1-color)*exp(depth)
	/// or -> color after reccursive call to getcolor, 1-(1-resultingColor) *
	/// expDepth

	if(intersect.intersectionSide != 0) {
		positive Ia = intersect.face[0];
		positive Ib = intersect.face[1];
		positive Ic = intersect.face[2];
		real Ka     = 1 - intersect.uv[0] - intersect.uv[1];
		real Kb     = intersect.uv[0];
		real Kc     = intersect.uv[1];
		vec16 Ma    = pool->materialPool + Ia * VEC16_SCALARS_COUNT;
		vec16 Mb    = pool->materialPool + Ib * VEC16_SCALARS_COUNT;
		vec16 Mc    = pool->materialPool + Ic * VEC16_SCALARS_COUNT;
		real point[VEC4_SCALARS_COUNT] = { Ka
			        * pool->vertexPool[Ia * VEC4_SCALARS_COUNT + 0]
			    + Kb * pool->vertexPool[Ib * VEC4_SCALARS_COUNT + 0]
			    + Kc * pool->vertexPool[Ic * VEC4_SCALARS_COUNT + 0],
			Ka * pool->vertexPool[Ia * VEC4_SCALARS_COUNT + 1]
			    + Kb * pool->vertexPool[Ib * VEC4_SCALARS_COUNT + 1]
			    + Kc * pool->vertexPool[Ic * VEC4_SCALARS_COUNT + 1],
			Ka * pool->vertexPool[Ia * VEC4_SCALARS_COUNT + 2]
			    + Kb * pool->vertexPool[Ib * VEC4_SCALARS_COUNT + 2]
			    + Kc * pool->vertexPool[Ic * VEC4_SCALARS_COUNT + 2],
			1 };

		/// Get material
		Material mat; // ambient=occlusion ambient, diffuse=light,
		              // specular/shininess=light_reflect,
		// reflect=%, refractiveIndex=, aepthAbsortion=exp(dist*.)

		real color[VEC3_SCALARS_COUNT] = { Ka * Ma[MAT_POOL_INDEX_RED]
			    + Kb * Mb[MAT_POOL_INDEX_RED] + Kc * Mc[MAT_POOL_INDEX_RED],
			Ka * Ma[MAT_POOL_INDEX_GREEN] + Kb * Mb[MAT_POOL_INDEX_GREEN]
			    + Kc * Mc[MAT_POOL_INDEX_GREEN],
			Ka * Ma[MAT_POOL_INDEX_BLUE] + Kb * Mb[MAT_POOL_INDEX_BLUE]
			    + Kc * Mc[MAT_POOL_INDEX_BLUE] };
		mat.ambient = Ka * Ma[MAT_POOL_INDEX_AMBIENT]
		    + Kb * Mb[MAT_POOL_INDEX_AMBIENT] + Kc * Mc[MAT_POOL_INDEX_AMBIENT];
		mat.diffuse = Ka * Ma[MAT_POOL_INDEX_DIFFUSE]
		    + Kb * Mb[MAT_POOL_INDEX_DIFFUSE] + Kc * Mc[MAT_POOL_INDEX_DIFFUSE];
		mat.specular = Ka * Ma[MAT_POOL_INDEX_SPECULAR]
		    + Kb * Mb[MAT_POOL_INDEX_SPECULAR]
		    + Kc * Mc[MAT_POOL_INDEX_SPECULAR];
		mat.shininess = Ka * Ma[MAT_POOL_INDEX_SHININESS]
		    + Kb * Mb[MAT_POOL_INDEX_SHININESS]
		    + Kc * Mc[MAT_POOL_INDEX_SHININESS];
		mat.emissive = Ka * Ma[MAT_POOL_INDEX_EMISSIVE]
		    + Kb * Mb[MAT_POOL_INDEX_EMISSIVE]
		    + Kc * Mc[MAT_POOL_INDEX_EMISSIVE];
		mat.reflect = Ka * Ma[MAT_POOL_INDEX_REFLECT]
		    + Kb * Mb[MAT_POOL_INDEX_REFLECT] + Kc * Mc[MAT_POOL_INDEX_REFLECT];
		mat.refractiveIndex = Ka * Ma[MAT_POOL_INDEX_REFRACTIVE]
		    + Kb * Mb[MAT_POOL_INDEX_REFRACTIVE]
		    + Kc * Mc[MAT_POOL_INDEX_REFRACTIVE];
		mat.depthAbsorbtion = Ka * Ma[MAT_POOL_INDEX_ABSORPTION]
		    + Kb * Mb[MAT_POOL_INDEX_ABSORPTION]
		    + Kc * Mc[MAT_POOL_INDEX_ABSORPTION];
		/// Normal
		real normal[VEC4_SCALARS_COUNT] = { Ka
			        * pool->normalPool[Ia * VEC4_SCALARS_COUNT + 0]
			    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + 0]
			    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + 0],
			Ka * pool->normalPool[Ia * VEC4_SCALARS_COUNT + 1]
			    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + 1]
			    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + 1],
			Ka * pool->normalPool[Ia * VEC4_SCALARS_COUNT + 2]
			    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + 2]
			    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + 2],
			0 };
		normalize(normal);
		multiply(normal, intersect.intersectionSide);
		/// Add texture
		//positive texture = intersect.face[3];

		colorOut[0] = color[0] * mat.emissive;
		colorOut[1] = color[1] * mat.emissive;
		colorOut[2] = color[2] * mat.emissive;
		// lights/shadow // depthAbsorption == +inf && reflect < 1 ;
		// specular=reflect
		// [*(1-refractRatioFromLight)] (ignore specular)

		if(mat.depthAbsorbtion > 1 - EPSILON)
			for(positive i = 0; i < pool->currentLightsCount; i++) {
				//real directIntensity = (1 - mat.reflect);

				bool shadow = false;
				if(pool->lightPool[i]->castShadow) {
					real lightDir[VEC4_SCALARS_COUNT];
					real dist = pool->lightPool[i]->getDirectionToSource(
					    point, lightDir);
					IntersectionData intersectLight;
					intersectTree(point, lightDir, tree, stack,
					    pool->vertexPool, intersect.face,
					    intersectLight); // TODO find one face (no ordering
					                     // needed)
					shadow = intersectLight.intersectionSide != 0
					    && intersectLight.depth < dist;
				}
				if(!shadow) {
					// lighting!
					pool->lightPool[i]->lighting(color, normal, point,
					    mat.ambient, mat.diffuse, mat.specular, mat.shininess,
					    colorOut);
					// todo ambient light has no shadow!
				}
			}

		// refractive // reflect < 1 && refractRatio > 0 && depthAbsorbtion <
		// +inf
		real refractDir[VEC4_SCALARS_COUNT];
		real refractRatio = 1;

		if(mat.depthAbsorbtion < (1 - EPSILON)) {
			refractRatio = refractRay(dir, normal, currentRefractiveIndex,
			    mat.refractiveIndex, refractDir);
			real refractIntensity = (1 - mat.reflect) * refractRatio;
			if(refractIntensity > EPSILON) {
				real refractColor[VEC3_SCALARS_COUNT];
				real refractDepth = 1.0;
				real absorption = mat.depthAbsorbtion == 0 ?
				    0 :
				    pow(mat.depthAbsorbtion,
				        refractDepth); // with material color
				getColor(point, refractDir, mat.refractiveIndex,
				    refractIntensity * (1 - absorption) * maxIntensity,
				    intersect.face, refractColor, &refractDepth, stack);
				colorOut[0] +=
				    refractColor[0] * refractIntensity * (1 - absorption);
				colorOut[1] +=
				    refractColor[1] * refractIntensity * (1 - absorption);
				colorOut[2] +=
				    refractColor[2] * refractIntensity * (1 - absorption);
			}
		}

		// reflect // reflect > 0 || (refractRation < 1 && depthAbsorbtion <
		// +inf)
		real reflectDir[VEC4_SCALARS_COUNT];
		reflect(dir, normal, reflectDir);
		real reflectIntensity =
		    mat.reflect + (1 - mat.reflect) * (1 - refractRatio);
		if(reflectIntensity > EPSILON) {
			real reflectColor[VEC3_SCALARS_COUNT];
			real reflectDepth;
			getColor(point, reflectDir, currentRefractiveIndex,
			    reflectIntensity * maxIntensity, intersect.face, reflectColor,
			    &reflectDepth, stack);
			colorOut[0] += reflectColor[0] * reflectIntensity;
			colorOut[1] += reflectColor[1] * reflectIntensity;
			colorOut[2] += reflectColor[2] * reflectIntensity;
		}
	}
	else {
		// skybox
	}

	return 0;
}

void RayTracer::render()
{
	if(tree != NULL)
		delete tree;
	pool->reset();
	imageBuffer->clear();

	sceneToPool.run(*scene, *pool, false);

	tree = new KDTree(*pool, pool->facePool, 0, pool->currentFacesCount);
	tree->build(*pool);
	tree->print(0);

	multithread.execute(threadRenderTask, this);
}

} // namespace proxo
