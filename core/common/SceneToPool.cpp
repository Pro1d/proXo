#include <cstdlib>
#include <cstring>
#include "SceneToPool.h"
#include "../math/Matrix.h"
#include "../model/TransformationStack.h"

SceneToPool::SceneToPool() : transformation(50) {

}

void SceneToPool::run(Scene & scene, Pool & pool) {
    Container ** contStackTop = containerStack;
    // push body
    *contStackTop = scene.body;
    contStackTop++;

    // push world
    *contStackTop = scene.world;
    contStackTop++;
    transformation.saveAndPreMult(scene.camera.matrix);

    while(contStackTop != containerStack) {
        contStackTop--;
        mat4 matrix = transformation.getMatrix();
        if(*contStackTop == NULL) {
            transformation.restore();
        }
        else {
            // TODO do not push object if not visible (circle/cube min)
            // Go trough object, light and containers
            for(positive i = (*contStackTop)->objectsCount; --i >= 0; ) {
                transformation.saveAndPreMult((*contStackTop)->objectMatrices[i]);
                objectToPool(*(*contStackTop)->objects[i], *(*contStackTop)->objectMaterials[i], pool);
                transformation.restore();
            }

            for(positive i = (*contStackTop)->lightsCount; --i >= 0; ) {
                transformation.saveAndPreMult((*contStackTop)->lightMatrices[i]);
                lightToPool(*(*contStackTop)->lights[i], pool);
                transformation.restore();
            }

            *contStackTop = NULL;
            contStackTop++;
            for(positive i = (*contStackTop)->containersCount; --i >= 0; ) {
                // push container
                transformation.pushMult((*contStackTop)->containerMatrices[i], matrix);
                *contStackTop = (*contStackTop)->containers[i];
                contStackTop++;
            }
        }
    }
}

void SceneToPool::objectToPool(Object & object, Material & material, Pool & pool) {
    positive vertexOffset = pool.currentVerticesCount;
    mat4 matrix = transformation.getMatrix();

    // Per vertex
    vec4 vertices = pool.vertexPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec4 normals = pool.normalPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec8 materials = pool.materialPool + vertexOffset * VEC8_SCALARS_COUNT;
    vec2 mappings = pool.mappingPool + vertexOffset * VEC2_SCALARS_COUNT;
    positive * textures = pool.texturePool + vertexOffset;

    for(positive i = 0; i < object.verticesCount; i++) {
        // apply matrix to vertices and normals
        multiplyMV(matrix, object.vertices+i*VEC4_SCALARS_COUNT, vertices);
        multiplyMV(matrix, object.normals+i*VEC4_SCALARS_COUNT, normals);
        vertices += VEC4_SCALARS_COUNT;
        normals += VEC4_SCALARS_COUNT;

        // copy colors and material
        memcpy(materials, object.colors+i*VEC4_SCALARS_COUNT, 3);
        materials[3] = material.ambient;
        materials[4] = material.diffuse;
        materials[5] = material.specular;
        materials[6] = material.shininess;
        materials[7] = material.emissive;
        materials += VEC8_SCALARS_COUNT;

        // copy colors, mappings and textures
        if(object.texture_mapping != NULL)
            memcpy(mappings, object.texture_mapping+i*VEC2_SCALARS_COUNT, VEC2_SIZE);
        *textures = object.texture_id;
        mappings += VEC2_SCALARS_COUNT;
        textures += 1;
    }

    pool.currentVerticesCount += object.verticesCount;


    // Per face
    positive * faces = pool.facePool + pool.currentFacesCount*3;

    positive * end = object.faces + object.facesCount * 3;
    for(positive * f = object.faces; f != end;) {
        // copy face's vertex indices and add offset
        *faces = *f + vertexOffset;
        ++f;
        ++faces;
        *faces = *f + vertexOffset;
        ++f;
        ++faces;
        *faces = *f + vertexOffset;
        ++f;
        ++faces;
    }

    pool.currentFacesCount += object.facesCount;
}

void SceneToPool::lightToPool(Light & light, Pool & pool) {
    light.transform(transformation.getMatrix());
    pool.lightPool[pool.currentLightsCount] = &light;
    pool.currentLightsCount++;
}
