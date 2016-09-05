#ifndef POOL_H
#define POOL_H


class Pool
{
    public:
        Pool(positive maxVertexCount, positive maxFaceCount);
        virtual ~Pool();

    protected:

    private:
        vec4 vertexPool;
        vec4 normalPool;
        vec4 colorPool;
        vec2 mappingPool;
        positive * texturePool;
        positive currentVerticesCount;
        positive * facePool;
        positive currentFacesCount;



};

#endif // POOL_H
