#include "BlurLight.h"
#include "RayTracer.h"
#include "Physic.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include <cmath>
#include <cstdio>
#include <limits>

namespace proxo {

#define EPSILON ((real) 1 / 255)

RayTracer::RayTracer(Buffer* imageBuffer, Scene* scene)
    : scene(scene),
      pool(NULL),
      tree(NULL),
      imageBuffer(imageBuffer),
      multithread(THREADS_COUNT),
      sceneToPool(multithread),
      depthMax(64)
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
	TreeStack treeStack(1024);
	MaterialStack matStack(256, 1.0, 0.0);

	real rayOrig[VEC4_SCALARS_COUNT] = { 0, 0, 0, 1 };
	real rayDir[VEC4_SCALARS_COUNT]  = { 0, 0, 0, 0 };

	Camera& cam(that->scene->camera);
	real xmin = cam.xmin;
	real w    = cam.xmax - xmin;
	real ymin = cam.ymin;
	real h    = cam.ymax - ymin;

	if(threadId == 0)
		printf("Rendering... 0%%\n");

	for(positive y = threadId; y < that->imageBuffer->height;
	    y += threadsCount) {
		for(positive x = 0; x < that->imageBuffer->width; x++) {
			// Build ray
			rayDir[0] = ((real) x / cam.screenWidth) * w + xmin;
			rayDir[1] = (1 - (real) y / cam.screenHeight) * h + ymin;
			rayDir[2] = -cam.zNear;
			normalize(rayDir);

			// Get associated pixel
			vec4 pxl = that->imageBuffer->getPtr(x, y);

			// Cast ray
			that->getColor(
			    rayOrig, rayDir, 1, NULL, pxl + 1, pxl, treeStack, matStack);

			// Cast more ray for depth of field
			positive count = 1;
			for(DepthOfField::OriginsIterator oi = that->dof_.begin();
			    oi != that->dof_.end(); ++oi, count++) {
				// Build ray
				real p[VEC3_SCALARS_COUNT], d;
				real ro[VEC4_SCALARS_COUNT], rd[VEC4_SCALARS_COUNT];
				that->dof_.getDirection(rayDir, oi, rd, ro);
				// Cast ray
				that->getColor(ro, rd, 1, NULL, p, &d, treeStack, matStack);
				// Add color to pixel
				pxl[1] += p[0];
				pxl[2] += p[1];
				pxl[3] += p[2];
			}

			// Pixel color is average of all rays
			pxl[1] /= count;
			pxl[2] /= count;
			pxl[3] /= count;
		}

		if(threadId == 0)
			printf("\033[1ARendering... %d%%\n",
			    y * 100 / that->imageBuffer->height);
	}

	if(threadId == 0)
		printf("\033[1ARendering... 100%%\n");
}

void RayTracer::getPointData(IntersectionData& intersect, Pool* pool,
    Material& mat, vec4 point, vec4 normal, vec3 color, vec3 emissive)
{
	// Vertex indices
	positive Ia = intersect.face[0];
	positive Ib = intersect.face[1];
	positive Ic = intersect.face[2];
	// Vertex weights
	real Ka = 1 - intersect.uv[0] - intersect.uv[1];
	real Kb = intersect.uv[0];
	real Kc = intersect.uv[1];
	// Vertex materials
	vec16 Ma = pool->materialPool + Ia * VEC16_SCALARS_COUNT;
	vec16 Mb = pool->materialPool + Ib * VEC16_SCALARS_COUNT;
	vec16 Mc = pool->materialPool + Ic * VEC16_SCALARS_COUNT;
	// Vertex positions
	vec4 Pa = pool->vertexPool + Ia * VEC4_SCALARS_COUNT;
	vec4 Pb = pool->vertexPool + Ib * VEC4_SCALARS_COUNT;
	vec4 Pc = pool->vertexPool + Ic * VEC4_SCALARS_COUNT;

	// Point 3D
	for(positive i = 0; i < 3; i++)
		point[i]   = Ka * Pa[i] + Kb * Pb[i] + Kc * Pc[i];
	point[3]       = 1;

	// ambient=occlusion, diffuse=direct light,
	// specular/shininess=light_reflect,
	// reflect=%, refractiveIndex=, depthAbsortion=exp(dist*.)

	// Fill material
	const positive r = Pool::MAT_INDEX_RED;
	for(positive i         = 0; i < 3; i++)
		color[i]           = Ka * Ma[r + i] + Kb * Mb[r + i] + Kc * Mc[r + i];
	mat.ambient            = Ma[Pool::MAT_INDEX_AMBIENT];
	mat.diffuse            = Ma[Pool::MAT_INDEX_DIFFUSE];
	mat.specular           = Ma[Pool::MAT_INDEX_SPECULAR];
	mat.shininess          = Ma[Pool::MAT_INDEX_SHININESS];
	mat.emissive           = Ma[Pool::MAT_INDEX_EMISSIVE];
	mat.refractiveIndex    = Ma[Pool::MAT_INDEX_REFRACTIVE];
	mat.depthAbsorption[0] = Ma[Pool::MAT_INDEX_ABSORPTION_RED];
	mat.depthAbsorption[1] = Ma[Pool::MAT_INDEX_ABSORPTION_GREEN];
	mat.depthAbsorption[2] = Ma[Pool::MAT_INDEX_ABSORPTION_BLUE];

	for(positive i  = 0; i < 3; i++)
		emissive[i] = color[i] * mat.emissive;

	/// Normal, might be overided by normal map
	for(positive i = 0; i < 3; i++)
		normal[i]  = Ka * pool->normalPool[Ia * VEC4_SCALARS_COUNT + i]
		    + Kb * pool->normalPool[Ib * VEC4_SCALARS_COUNT + i]
		    + Kc * pool->normalPool[Ic * VEC4_SCALARS_COUNT + i];
	normal[3] = 0;
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

		// Diffuse color
		if(texture->hasField(Texture::FLAG_DIFFUSE_RGB)) {
			vec3 diffuse_tex =
			    texel + texture->getFieldOffset(Texture::DIFFUSE_RGB);
			color[0] *= diffuse_tex[0];
			color[1] *= diffuse_tex[1];
			color[2] *= diffuse_tex[2];
		}
		// Ambient occlusion
		if(texture->hasField(Texture::FLAG_AMBIENT_I)) {
			real ambient_tex =
			    texel[texture->getFieldOffset(Texture::AMBIENT_I)];
			mat.ambient *= ambient_tex;
		}
		// Specular mask
		if(texture->hasField(Texture::FLAG_SPECULAR_I)) {
			real specular_tex =
			    texel[texture->getFieldOffset(Texture::SPECULAR_I)];
			mat.specular *= specular_tex;
		}
		// Emissive color
		if(texture->hasField(Texture::FLAG_EMISSIVE_RGB)) {
			vec3 emissive_tex =
			    texel + texture->getFieldOffset(Texture::EMISSIVE_RGB);
			for(positive i = 0; i < 3; i++)
				emissive[i] += emissive_tex[i] * (1 - emissive[i]);
		}
		// Shininess mask
		if(texture->hasField(Texture::FLAG_SHININESS_I)) {
			real shininess_tex =
			    texel[texture->getFieldOffset(Texture::SHININESS_I)];
			mat.shininess *= shininess_tex;
		}
		// Normal map
		if(texture->hasField(Texture::FLAG_NORMAL_XYZ)) {
			vec3 normal_tex =
			    texel + texture->getFieldOffset(Texture::NORMAL_XYZ);
			Texture::normalMapToWorldCoord(
			    Pa, Pb, Pc, mapa, mapb, mapc, normal_tex, normal);
		}
	}

	multiply(normal, intersect.intersectionSide);
}

void RayTracer::getLightingColor(vec3 color, vec4 point, vec4 normal,
    Material& mat, IntersectionData& intersect, TreeStack& treeStack,
    MaterialStack& matStack, vec3 colorOut)
{
	SortedIntersectionsData intersectLight;
	BlurLight blurLight;

	for(positive i = 0; i < pool->currentLightsCount; i++) {
		Light* light = pool->lightPool[i];

		// Get the potential lighting color
		real lightingColor[VEC3_SCALARS_COUNT] = { 0, 0, 0 };
		real ambient = mat.ambient * (1 - mat.specular);
		real diffuse = mat.diffuse * (1 - mat.specular);
		// TODO compute fresnel coef for reflection/specular
		// intensity (translucent object)
		light->lighting(color, normal, point, ambient, diffuse, mat.specular,
		    mat.shininess, lightingColor);

		if(lightingColor[0] > EPSILON || lightingColor[1] > EPSILON
		    || lightingColor[2] > EPSILON) {
			
			real lightDir[VEC4_SCALARS_COUNT];
			real distToLight = light->getDirectionToSource(point, lightDir);
			
			BlurLight::Iterator it = blurLight.set(lightDir, distToLight, light->transformedRadius);
			
			real finalFilter[VEC3_SCALARS_COUNT] = {0,0,0};
			positive count = 1;
			
			// Test light occlusion (shadow)
			if(light->castShadow) {
				for(count = 0; !it.end(); count++, it.incr()) {
					real dir[VEC3_SCALARS_COUNT];
					blurLight.getDirection(it, dir);

					intersectTreeLighting(point, dir, tree, treeStack,
							pool->vertexPool, pool->materialPool, intersect.face, distToLight,
							intersectLight);

					if(!intersectLight.containsOpaqueFace) {
						real filter[VEC3_SCALARS_COUNT] = {1,1,1};
						real prevDepth = 0;
						MaterialStack ms(matStack);
						if(!intersectLight.empty()) {
							// Go trough translucent material to reach the light source
							while(!intersectLight.empty()) {
								IntersectionData intersect = intersectLight.top();
								intersectLight.pop();

								const real* ab = ms.top().absorption;
								real depth = intersect.depth - prevDepth;
								filter[0] *= ab[0] == 0 ? 1 : pow(1-ab[0], depth);
								filter[1] *= ab[1] == 0 ? 1 : pow(1-ab[1], depth);
								filter[2] *= ab[2] == 0 ? 1 : pow(1-ab[2], depth);

								// update material stack
								if(intersect.intersectionSide == 1) {
									vec16 m = pool->materialPool + intersect.face[0] * VEC16_SCALARS_COUNT;
									ms.push(m[Pool::MAT_INDEX_REFRACTIVE], m + Pool::MAT_INDEX_ABSORPTION_RED);
								}
								else
									ms.pop();

								prevDepth = intersect.depth;
							}
						}

						const real* ab = ms.top().absorption;
						real depth = distToLight - prevDepth;
						filter[0] *= ab[0] == 0 ? 1 : pow(1-ab[0], depth);
						filter[1] *= ab[1] == 0 ? 1 : pow(1-ab[1], depth);
						filter[2] *= ab[2] == 0 ? 1 : pow(1-ab[2], depth);
						finalFilter[0] += filter[0];
						finalFilter[1] += filter[1];
						finalFilter[2] += filter[2];
					}
				}
			}
			else {
				finalFilter[0] = 1;
				finalFilter[1] = 1;
				finalFilter[2] = 1;
			}

			// apply lighting
			//if(!intersectLight.containsOpaqueFace) {
				// lighting!
				colorOut[0] += lightingColor[0] * finalFilter[0]/count;
				colorOut[1] += lightingColor[1] * finalFilter[1]/count;
				colorOut[2] += lightingColor[2] * finalFilter[2]/count;
			//}
		}
	}
}

void RayTracer::getRefractionColor(vec4 point, vec4 normal, vec3 dir,
    Material& mat, IntersectionData& intersect, TreeStack& treeStack,
    MaterialStack& matStack, real maxIntensity, vec3 colorOut,
    real& refractRatioOut, positive depth)
{
	InnerMaterial innerMat = matStack.top();

	if(intersect.intersectionSide == 1)
		matStack.push(mat.refractiveIndex, mat.depthAbsorption);
	else
		matStack.pop();

	real currentRefractiveIndex = innerMat.refractive_index;

	real refractDir[VEC4_SCALARS_COUNT];
	refractRatioOut = refractRay(dir, normal, currentRefractiveIndex,
	    matStack.top().refractive_index, refractDir);
	real refractIntensity = (1 - mat.specular) * refractRatioOut;

	if(refractIntensity > EPSILON) {
		real refractColor[VEC3_SCALARS_COUNT];
		real refractDepth;

		// Cast refracted ray
		getColor(point, refractDir, refractIntensity * maxIntensity,
		    intersect.face, refractColor, &refractDepth, treeStack, matStack,
		    depth + 1);

		const real* ab     = matStack.top().absorption;
		real penetrating_r = ab[0] == 0 ? 1 : pow(1 - ab[0], refractDepth);
		real penetrating_g = ab[1] == 0 ? 1 : pow(1 - ab[1], refractDepth);
		real penetrating_b = ab[2] == 0 ? 1 : pow(1 - ab[2], refractDepth);
		colorOut[0] += refractColor[0] * refractIntensity * penetrating_r;
		colorOut[1] += refractColor[1] * refractIntensity * penetrating_g;
		colorOut[2] += refractColor[2] * refractIntensity * penetrating_b;
	}

	if(intersect.intersectionSide == 1)
		matStack.pop();
	else
		matStack.push(innerMat);
}

void RayTracer::getReflectionColor(vec4 point, vec4 normal, vec3 dir,
    real refractRatio, Material& mat, IntersectionData& intersect,
    TreeStack& treeStack, MaterialStack& matStack, real maxIntensity,
    vec3 colorOut, positive depth)
{
	real reflectDir[VEC4_SCALARS_COUNT];
	reflect(dir, normal, reflectDir);
	real reflectIntensity = 1 - refractRatio * (1 - mat.specular);
	if(reflectIntensity > EPSILON) {
		real reflectColor[VEC3_SCALARS_COUNT];
		real reflectDepth;
		// Cast refletc ray
		getColor(point, reflectDir, reflectIntensity * maxIntensity,
		    intersect.face, reflectColor, &reflectDepth, treeStack, matStack,
		    depth + 1);
		colorOut[0] += reflectColor[0] * reflectIntensity;
		colorOut[1] += reflectColor[1] * reflectIntensity;
		colorOut[2] += reflectColor[2] * reflectIntensity;
	}
}

void RayTracer::getColor(vec3 orig, vec3 dir, real maxIntensity,
    positive* lastFace, vec3 colorOut, real* depthOut, TreeStack& treeStack,
    MaterialStack& matStack, positive depth)
{
	colorOut[0] = 0;
	colorOut[1] = 0;
	colorOut[2] = 0;

	// Avoid infinite reccursion
	if(maxIntensity < (real) 1 / 255 || depth >= depthMax)
		return;

	// Get ray first intersection
	IntersectionData intersect;
	intersectTree(
	    orig, dir, tree, treeStack, pool->vertexPool, lastFace, intersect);

	/// TODO translucent, couleur perçue absorbée par épaisseur d'objet pas
	/// complètement transparent
	/// -> color when go out of object, 1-(1-color)*exp(depth)
	/// or -> color after reccursive call to getcolor, 1-(1-resultingColor) *
	/// expDepth

	if(intersect.intersectionSide != 0) {
		real point[VEC4_SCALARS_COUNT];
		real normal[VEC4_SCALARS_COUNT];
		real color[VEC3_SCALARS_COUNT];
		real emissive[VEC3_SCALARS_COUNT];
		Material mat;
		getPointData(intersect, pool, mat, point, normal, color, emissive);

		colorOut[0] = emissive[0];
		colorOut[1] = emissive[1];
		colorOut[2] = emissive[2];
		*depthOut   = intersect.depth;

		bool isOpaque = mat.depthAbsorption[0] > 1 - EPSILON * EPSILON
		    && mat.depthAbsorption[1] > 1 - EPSILON * EPSILON
		    && mat.depthAbsorption[2] > 1 - EPSILON * EPSILON;

		// lights/shadow // depthAbsorption == 1
		// TODO allow translucent object to receive specular light (with fresnel
		// coeffs)
		if(isOpaque) {
			getLightingColor(color, point, normal, mat, intersect, treeStack,
			    matStack, colorOut);
		}

		// refractive // reflect < 1 && refractRatio > 0 && depthAbsorbtion < 1
		real refractRatio = 1;

		if(!isOpaque) {
			getRefractionColor(point, normal, dir, mat, intersect, treeStack,
			    matStack, maxIntensity, colorOut, refractRatio, depth);
		}

		// reflect // reflect > 0 || (refractRation < 1 && depthAbsorbtion < 1)
		getReflectionColor(point, normal, dir, refractRatio, mat, intersect,
		    treeStack, matStack, maxIntensity, colorOut, depth);
	}
	else {
		// skybox
		colorOut[0] = 0;
		colorOut[1] = 0;
		colorOut[2] = 0;
		*depthOut   = std::numeric_limits<real>::infinity();
	}
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
		TreeStack treeStack(64);
		real orig[VEC3_SCALARS_COUNT] = { 0, 0, 0 };
		real dir[VEC3_SCALARS_COUNT]  = { 0, 0, -1 };
		intersectTree(
		    orig, dir, tree, treeStack, pool->vertexPool, NULL, intersect);
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

	printf("Render target: %dx%d\n", (integer) scene->camera.screenWidth,
	    (integer) scene->camera.screenHeight);
	printf("DoF: %d rays\n", dof_.getRaysCount());

	multithread.execute(RayTracer::threadRenderTask, this);
}

} // namespace proxo
