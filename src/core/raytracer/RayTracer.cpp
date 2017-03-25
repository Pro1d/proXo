#include "RayTracer.h"
#include "DepthOfField.h"
#include "Intersection.h"
#include "Physic.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include <cstdio>

namespace proxo {

#define EPSILON ((real) 1 / 255)

RayTracer::RayTracer(Buffer* imageBuffer, Scene* scene)
    : scene(scene),
      pool(NULL),
      tree(NULL),
      imageBuffer(imageBuffer),
      multithread(THREADS_COUNT),
      sceneToPool(multithread)
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
	pool = new Pool(vertices, faces, lights, scene->objects.size());
}

void RayTracer::threadRenderTask(
    void* data, positive threadId, positive threadsCount)
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

	if(threadId == 0)
		printf("Rendering... 0%%\n");

	for(positive y = threadId; y < that->imageBuffer->height;
	    y += threadsCount) {

		for(positive x = 0; x < that->imageBuffer->width; x++) {
			rayDir[0] = ((real) x / cam.screenWidth) * dx + xmin;
			rayDir[1] = (1 - (real) y / cam.screenHeight) * dy + ymin;
			rayDir[2] = -cam.zNear;
			normalize(rayDir);

			vec4 pxl = that->imageBuffer->getPtr(x, y);

			that->getColor(rayOrig, rayDir, 1, 1, NULL, pxl + 1, pxl, stack);

			positive count = 1;
			for(DepthOfField::OriginsIterator oi = that->dof_.begin();
			    oi != that->dof_.end(); ++oi, count++) {
				real p[VEC3_SCALARS_COUNT], d;
				real ro[VEC4_SCALARS_COUNT], rd[VEC4_SCALARS_COUNT];
				that->dof_.getDirection(rayDir, oi, rd, ro);
				that->getColor(ro, rd, 1, 1, NULL, p, &d, stack);
				pxl[1] += p[0];
				pxl[2] += p[1];
				pxl[3] += p[2];
			}

			pxl[1] /= count;
			pxl[2] /= count;
			pxl[3] /= count;
		}

		if(threadId == 0)
			printf("\033[1ARendering... %d%%\n",
			    y * 100 / that->imageBuffer->height);
	}
}

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
		vec4 Pa     = pool->vertexPool + Ia * VEC4_SCALARS_COUNT;
		vec4 Pb     = pool->vertexPool + Ib * VEC4_SCALARS_COUNT;
		vec4 Pc     = pool->vertexPool + Ic * VEC4_SCALARS_COUNT;

		real point[VEC4_SCALARS_COUNT] = { Ka * Pa[0] + Kb * Pb[0] + Kc * Pc[0],
			Ka * Pa[1] + Kb * Pb[1] + Kc * Pc[1],
			Ka * Pa[2] + Kb * Pb[2] + Kc * Pc[2], 1 };

		/// Get material
		Material mat; // ambient=occlusion, diffuse=direct light,
		// specular/shininess=light_reflect,
		// reflect=%, refractiveIndex=, depthAbsortion=exp(dist*.)

		real color[VEC3_SCALARS_COUNT] = { Ka * Ma[Pool::MAT_INDEX_RED]
			    + Kb * Mb[Pool::MAT_INDEX_RED] + Kc * Mc[Pool::MAT_INDEX_RED],
			Ka * Ma[Pool::MAT_INDEX_GREEN] + Kb * Mb[Pool::MAT_INDEX_GREEN]
			    + Kc * Mc[Pool::MAT_INDEX_GREEN],
			Ka * Ma[Pool::MAT_INDEX_BLUE] + Kb * Mb[Pool::MAT_INDEX_BLUE]
			    + Kc * Mc[Pool::MAT_INDEX_BLUE] };
		mat.ambient = Ma[Pool::MAT_INDEX_AMBIENT]; /*
		     + Kb * Mb[Pool::MAT_INDEX_AMBIENT]
		     + Kc * Mc[Pool::MAT_INDEX_AMBIENT];*/
		mat.diffuse = Ma[Pool::MAT_INDEX_DIFFUSE]; /*
		     + Kb * Mb[Pool::MAT_INDEX_DIFFUSE]
		     + Kc * Mc[Pool::MAT_INDEX_DIFFUSE];*/
		mat.specular = Ma[Pool::MAT_INDEX_SPECULAR]; /*
		     + Kb * Mb[Pool::MAT_INDEX_SPECULAR]
		     + Kc * Mc[Pool::MAT_INDEX_SPECULAR];*/
		mat.shininess = Ma[Pool::MAT_INDEX_SHININESS]; /*
		     + Kb * Mb[Pool::MAT_INDEX_SHININESS]
		     + Kc * Mc[Pool::MAT_INDEX_SHININESS];*/
		mat.emissive = Ma[Pool::MAT_INDEX_EMISSIVE]; /*
		     + Kb * Mb[Pool::MAT_INDEX_EMISSIVE]
		     + Kc * Mc[Pool::MAT_INDEX_EMISSIVE];*/
		// TODO reflect = specular; remove reflect
		mat.reflect = Ma[Pool::MAT_INDEX_REFLECT]; /*
		     + Kb * Mb[Pool::MAT_INDEX_REFLECT]
		     + Kc * Mc[Pool::MAT_INDEX_REFLECT];*/
		mat.refractiveIndex = Ma[Pool::MAT_INDEX_REFRACTIVE]; /*
		     + Kb * Mb[Pool::MAT_INDEX_REFRACTIVE]
		     + Kc * Mc[Pool::MAT_INDEX_REFRACTIVE];*/
		// TODO Absorption RGB
		mat.depthAbsorbtion = Ma[Pool::MAT_INDEX_ABSORPTION]; /*
		     + Kb * Mb[Pool::MAT_INDEX_ABSORPTION]
		     + Kc * Mc[Pool::MAT_INDEX_ABSORPTION];*/
		real emissive[VEC3_SCALARS_COUNT] = { color[0] * mat.emissive,
			color[1] * mat.emissive, color[2] * mat.emissive };

		/// Normal, might be overided by normal map
		real normal[VEC4_SCALARS_COUNT] = { 0, 0, 0, 0 };
		normal[0] = Ka * pool->normalPool[Ia * VEC4_SCALARS_COUNT + 0]
		    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + 0]
		    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + 0];
		normal[1] = Ka * pool->normalPool[Ia * VEC4_SCALARS_COUNT + 1]
		    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + 1]
		    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + 1];
		normal[2] = Ka * pool->normalPool[Ia * VEC4_SCALARS_COUNT + 2]
		    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + 2]
		    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + 2];
		normalize(normal);

		/// Textures
		Texture* texture = pool->texturePool[intersect.face[3]];
		if(texture != NULL) {
			vec2 mapa = pool->mappingPool + Ia * VEC2_SCALARS_COUNT;
			vec2 mapb = pool->mappingPool + Ib * VEC2_SCALARS_COUNT;
			vec2 mapc = pool->mappingPool + Ic * VEC2_SCALARS_COUNT;
			real u    = Ka * mapa[0] + Kb * mapb[0] + Kc * mapc[0];
			real v    = Ka * mapa[1] + Kb * mapb[1] + Kc * mapc[1];

			real* texel = texture->getData(u, v);

			if(texture->hasField(Texture::FLAG_DIFFUSE_RGB)) {
				vec3 diffuse_tex =
				    texel + texture->getFieldOffset(Texture::DIFFUSE_RGB);
				color[0] *= diffuse_tex[0];
				color[1] *= diffuse_tex[1];
				color[2] *= diffuse_tex[2];
			}
			if(texture->hasField(Texture::FLAG_AMBIENT_I)) {
				real ambient_tex =
				    texel[texture->getFieldOffset(Texture::AMBIENT_I)];
				mat.ambient *= ambient_tex;
			}
			if(texture->hasField(Texture::FLAG_SPECULAR_I)) {
				real specular_tex =
				    texel[texture->getFieldOffset(Texture::SPECULAR_I)];
				mat.specular *= specular_tex;
			}
			if(texture->hasField(Texture::FLAG_EMISSIVE_RGB)) {
				vec3 emissive_tex =
				    texel + texture->getFieldOffset(Texture::EMISSIVE_RGB);
				emissive[0] += emissive_tex[0] * (1 - emissive[0]);
				emissive[1] += emissive_tex[1] * (1 - emissive[1]);
				emissive[2] += emissive_tex[2] * (1 - emissive[2]);
			}
			if(texture->hasField(Texture::FLAG_SHININESS_I)) {
				real shininess_tex =
				    texel[texture->getFieldOffset(Texture::SHININESS_I)];
				mat.shininess = shininess_tex;
			}
			if(texture->hasField(Texture::FLAG_NORMAL_XYZ)) {
				vec3 normal_tex =
				    texel + texture->getFieldOffset(Texture::NORMAL_XYZ);
				Texture::normalMapToWorldCoord(
				    Pa, Pb, Pc, mapa, mapb, mapc, normal_tex, normal);
			}
		}

		multiply(normal, intersect.intersectionSide);

		colorOut[0] = emissive[0];
		colorOut[1] = emissive[1];
		colorOut[2] = emissive[2];

		// lights/shadow // depthAbsorption == 1 && reflect < 1 ;
		if(mat.depthAbsorbtion > 1 - EPSILON)
			for(positive i = 0; i < pool->currentLightsCount; i++) {
				bool shadow       = false;
				Light* light      = pool->lightPool[i];

				// Get the potential lighting color
				real lightingColor[VEC3_SCALARS_COUNT] = { 0, 0, 0 };
				real ambient = mat.ambient * (1 - mat.reflect);
				real diffuse = mat.diffuse * (1 - mat.reflect);
				// TODO compute fresnel coef for reflection/specular
				// intensity (translucent object)
				light->lighting(color, normal, point, ambient, diffuse,
				    mat.specular, mat.shininess, lightingColor);

				if(lightingColor[0] > EPSILON || lightingColor[1] > EPSILON
				    || lightingColor[2] > EPSILON) {

					// Test light occlusion (shadow)
					if(light->castShadow) {
						real lightDir[VEC4_SCALARS_COUNT];
						real dist =
						    light->getDirectionToSource(point, lightDir);
						// TODO path to light in straight line with absorption
						IntersectionData intersectLight;
						intersectTree(point, lightDir, tree, stack,
						    pool->vertexPool, intersect.face, intersectLight);
						shadow = intersectLight.intersectionSide != 0
						    && intersectLight.depth < dist;
					}

					// apply lighting
					if(!shadow) {
						// lighting!
						colorOut[0] += lightingColor[0];
						colorOut[1] += lightingColor[1];
						colorOut[2] += lightingColor[2];
					}
				}
			}

		// refractive // reflect < 1 && refractRatio > 0 && depthAbsorbtion < 1
		real refractDir[VEC4_SCALARS_COUNT];
		real refractRatio = 1;

		if(mat.depthAbsorbtion < (1 - EPSILON)) {
			refractRatio = refractRay(dir, normal, currentRefractiveIndex,
			    mat.refractiveIndex, refractDir);
			real refractIntensity = (1 - mat.reflect) * refractRatio;
			if(refractIntensity > EPSILON) {
				real refractColor[VEC3_SCALARS_COUNT];
				real refractDepth = 1.0;
				real absorption   = mat.depthAbsorbtion == 0 ?
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

		// reflect // reflect > 0 || (refractRation < 1 && depthAbsorbtion < 1)
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
	scene->printSize();

	tree = new KDTree(*pool, pool->facePool, 0, pool->currentFacesCount);
	tree->build(*pool);

	// Auto focus enabled: find distance to nearest object in middle of the
	// screen
	if(scene->camera.autofocus) {
		IntersectionData intersect;
		TreeStack stack(512);
		real orig[VEC3_SCALARS_COUNT] = { 0, 0, 0 };
		real dir[VEC3_SCALARS_COUNT]  = { 0, 0, -1 };
		intersectTree(
		    orig, dir, tree, stack, pool->vertexPool, NULL, intersect);
		if(intersect.intersectionSide == 0)
			dof_.setFocusDistance(scene->camera.distanceFocus);
		else
			dof_.setFocusDistance(intersect.depth);
	}
	else {
		dof_.setFocusDistance(scene->camera.distanceFocus);
	}
	dof_.setAperture(scene->camera.aperture);
	dof_.updatePattern(5, 2);

	multithread.execute(RayTracer::threadRenderTask, this);
}

} // namespace proxo
