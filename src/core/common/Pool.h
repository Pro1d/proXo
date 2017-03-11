#ifndef POOL_H
#define POOL_H

#include "core/math/type.h"
#include "core/model/Light.h"
#include "core/model/Texture.h"

namespace proxo {

class Pool {
public:
	enum {
		MAT_INDEX_RED,
		MAT_INDEX_GREEN,
		MAT_INDEX_BLUE,
		MAT_INDEX_AMBIENT,
		MAT_INDEX_DIFFUSE,
		MAT_INDEX_SPECULAR,
		MAT_INDEX_SHININESS,
		MAT_INDEX_EMISSIVE,
		MAT_INDEX_REFLECT,
		MAT_INDEX_REFRACTIVE,
		MAT_INDEX_ABSORPTION
	};

	Pool(positive maxVerticesCount, positive maxFacesCount,
	    positive maxLightsCount, positive maxTexturesCount);
	virtual ~Pool();
	void reset();

	vec4 vertexPool;
	vec4 normalPool;
	vec16 materialPool; // r, g, b, ambient, diffuse, specular, shininess,
	// emissive, reflect,
	// refractiveIndex, depthAbsorption
	vec2 mappingPool;
	positive currentVerticesCount;
	Texture** texturePool;
	positive currentTexturesCount;
	positive* facePool; // vertex1, vertex2, vertex3, texture_id
	positive currentFacesCount;
	Light** lightPool;
	positive currentLightsCount;
};

} // namespace proxo

#endif // POOL_H
