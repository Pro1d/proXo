#ifndef POOL_H
#define POOL_H

#include "../math/type.h"
#include "../model/Light.h"

class Pool
{
    public:
        Pool(positive maxVerticesCount, positive maxFacesCount, positive maxLightsCount);
        virtual ~Pool();
        void reset();

        vec4 vertexPool;
        vec4 normalPool;
        vec8 materialPool; // r, g, b, ambient, diffuse, specular, shininess, emissive
        vec2 mappingPool;
        positive currentVerticesCount;
        //positive * texturePool;
        positive * facePool;
        positive currentFacesCount;
        Light ** lightPool;
        positive currentLightsCount;
};

#endif // POOL_H
