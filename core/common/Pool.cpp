#include "Pool.h"

Pool::Pool(positive maxVerticesCount, positive maxFacesCount, positive maxLightsCount) :
    vertexPool(new real[maxVerticesCount*VEC4_SCALARS_COUNT]),
    normalPool(new real[maxVerticesCount*VEC4_SCALARS_COUNT]),
    materialPool(new real[maxVerticesCount*VEC8_SCALARS_COUNT]),
    mappingPool(new real[maxVerticesCount*VEC2_SCALARS_COUNT]),
    texturePool(new positive[maxVerticesCount]),
    currentVerticesCount(0),
    facePool(new positive[maxFacesCount]),
    currentFacesCount(0),
    lightPool(new Light*[maxLightsCount]),
    currentLightsCount(0)
{
    //ctor
}

Pool::~Pool()
{
    delete[] vertexPool;
    delete[] normalPool;
    delete[] materialPool;
    delete[] mappingPool;
    delete[] texturePool;
    delete[] facePool;
    delete[] lightPool;
}
