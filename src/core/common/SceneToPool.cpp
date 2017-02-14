#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "SceneToPool.h"
#include "core/math/Matrix.h"
#include "core/math/Vector.h"
#include "core/model/TransformationStack.h"

SceneToPool::SceneToPool() : transformation(50) {

}

void SceneToPool::run(Scene & scene, Pool & pool, bool skipNotVisibleObject) {
    Container ** contStackTop = containerStack;
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
        Container * top = *(--contStackTop);
        mat4 matrix = transformation.getMatrix();

        if(top == NULL) {
            transformation.restore();
        }
        else {
            // Go trough object, light and containers
            for(integer i = top->objectsCount; --i >= 0; ) {
                transformation.saveAndPostMult(top->objectMatrices[i]);
                objectToPool(*top->objects[i], *top->objectMaterials[i], pool, scene.camera, skipNotVisibleObject);
                transformation.restore();
            }

            for(integer i = top->lightsCount; --i >= 0; ) {
                transformation.saveAndPostMult(top->lightMatrices[i]);
                lightToPool(*top->lights[i], pool);
                transformation.restore();
            }

            *contStackTop = NULL;
            contStackTop++;
            for(integer i = top->containersCount; --i >= 0; ) {
                // push container
                transformation.pushMult(matrix, top->containerMatrices[i]);
                *contStackTop = top->containers[i];
                contStackTop++;
            }
        }
    }
}

void SceneToPool::objectToPool(Object & object, Material & material, Pool & pool, Camera & camera, bool skipNotVisibleObject) {
    positive vertexOffset = pool.currentVerticesCount;
    mat4 matrix = transformation.getMatrix();

    if(skipNotVisibleObject) {
        // Do not add object to pool if object is not visible
        real center[VEC4_SCALARS_COUNT];
        multiplyMV(matrix, object.boundingSphereCenter, center);
        real radius = object.boundingSphereRadius * getMatrixScale(matrix);
        if(!camera.isShpereVisible(center, radius))
            return;
    }

    // Per vertex
    vec4 vertices = pool.vertexPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec4 normals = pool.normalPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec16 materials = pool.materialPool + vertexOffset * VEC16_SCALARS_COUNT;
    vec2 mappings = pool.mappingPool + vertexOffset * VEC2_SCALARS_COUNT;

    for(positive i = 0; i < object.verticesCount; i++) {
        // apply matrix to vertices and normals
        multiplyNoNormalizeMV(matrix, object.vertices+i*VEC4_SCALARS_COUNT, vertices);
        multiplyNoNormalizeMV(matrix, object.normals+i*VEC4_SCALARS_COUNT, normals);
        normalize(normals);
        /*if(i == 0) {
            printf("%f %f %f %f n %f %f %f %f\n", vertices[0], vertices[1], vertices[2], vertices[3]
                   , normals[0], normals[1], normals[2], normals[3]);
        }*/

        vertices += VEC4_SCALARS_COUNT;
        normals += VEC4_SCALARS_COUNT;

        // copy colors and material
        if(object.colors != NULL)
            memcpy(materials, object.colors+i*VEC4_SCALARS_COUNT, 3*sizeof(real));
        else {
            materials[0] = 1;
            materials[1] = 1;
            materials[2] = 1;
        }
        materials[MAT_POOL_INDEX_AMBIENT] = material.ambient;
        materials[MAT_POOL_INDEX_DIFFUSE] = material.diffuse;
        materials[MAT_POOL_INDEX_SPECULAR] = material.specular;
        materials[MAT_POOL_INDEX_SHININESS] = material.shininess;
        materials[MAT_POOL_INDEX_EMISSIVE] = material.emissive;
        materials[MAT_POOL_INDEX_REFLECT] = material.reflect;
        materials[MAT_POOL_INDEX_REFRACTIVE] = material.refractiveIndex;
        materials[MAT_POOL_INDEX_ABSORPTION] = material.depthAbsorbtion;
        materials += VEC16_SCALARS_COUNT;

        // copy colors, mappings and textures
        if(object.texture_mapping != NULL)
            memcpy(mappings, object.texture_mapping+i*VEC2_SCALARS_COUNT, VEC2_SIZE);
        mappings += VEC2_SCALARS_COUNT;
    }

    pool.currentVerticesCount += object.verticesCount;


    // Per face
    positive * faces = pool.facePool + pool.currentFacesCount*4;

    positive * end = object.faces + object.facesCount * 3;
    for(positive * f = object.faces; f != end; f += 3, faces += 4) {
        // copy face's vertex indices and add offset
        faces[0] = f[0] + vertexOffset;
        faces[1] = f[1] + vertexOffset;
        faces[2] = f[2] + vertexOffset;
        faces[3] = object.texture_id;
    }

    pool.currentFacesCount += object.facesCount;
}

void SceneToPool::lightToPool(Light & light, Pool & pool) {
    light.transform(transformation.getMatrix());
    pool.lightPool[pool.currentLightsCount] = &light;
    pool.currentLightsCount++;
}
