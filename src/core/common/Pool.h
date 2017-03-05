#ifndef POOL_H
#define POOL_H

#include "core/math/type.h"
#include "core/model/Light.h"

enum {
	MAT_POOL_INDEX_RED,
	MAT_POOL_INDEX_GREEN,
	MAT_POOL_INDEX_BLUE,
	MAT_POOL_INDEX_AMBIENT,
	MAT_POOL_INDEX_DIFFUSE,
	MAT_POOL_INDEX_SPECULAR,
	MAT_POOL_INDEX_SHININESS,
	MAT_POOL_INDEX_EMISSIVE,
	MAT_POOL_INDEX_REFLECT,
	MAT_POOL_INDEX_REFRACTIVE,
	MAT_POOL_INDEX_ABSORPTION
};

class Pool {
public:
	Pool(positive maxVerticesCount, positive maxFacesCount,
	    positive maxLightsCount);
	virtual ~Pool();
	void reset();

	vec4 vertexPool;
	vec4 normalPool;
	vec16 materialPool; // r, g, b, ambient, diffuse, specular, shininess,
	                    // emissive, reflect,
	// refractiveIndex, depthAbsorption
	vec2 mappingPool;
	positive currentVerticesCount;
	// positive * texturePool;
	positive* facePool; // vertex1, vertex2, vertex3, texture_id
	positive currentFacesCount;
	Light** lightPool;
	positive currentLightsCount;
};

#endif // POOL_H
