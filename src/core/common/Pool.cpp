#include "Pool.h"

Pool::Pool(positive maxVerticesCount, positive maxFacesCount, positive maxLightsCount) :
    vertexPool(new real[maxVerticesCount*VEC4_SCALARS_COUNT]),
    normalPool(new real[maxVerticesCount*VEC4_SCALARS_COUNT]),
    materialPool(new real[maxVerticesCount*VEC16_SCALARS_COUNT]),
    mappingPool(new real[maxVerticesCount*VEC2_SCALARS_COUNT]),
    currentVerticesCount(0),
    facePool(new positive[maxFacesCount*4]),
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
    delete[] facePool;
    delete[] lightPool;
}

void Pool::reset() {
    currentVerticesCount = 0;
    currentFacesCount = 0;
    currentLightsCount = 0;
}
