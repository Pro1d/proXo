#include "SceneToPool.h"

SceneToPool::SceneToPool() : transformation(50) {

}

void SceneToPool::run(Scene & scene, Pool & pool) {
    Container ** contStackTop = containerStack;
    // push body
    *contStackTop = &scene.body;
    contStackTop++;

    // push world
    *contStackTop = &scene.world;
    contStackTop++;
    transformation.pushMult(camera.matrix);

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
                transformation.saveAndPreMult((*contStackTop)->objectMatrices+i*MAT4_SCALARS_COUNT);
                objectToPool((*contStackTop)->objects[i], pool);
                transformation.restore();
            }

            for(positive i = (*contStackTop)->lightsCount; --i >= 0; ) {
                transformation.saveAndPreMult((*contStackTop)->lightMatrices+i*MAT4_SCALARS_COUNT);
                lightToPool((*contStackTop)->lights[i], pool);
                transformation.restore();
            }

            *contStackTop = NULL;
            contStackTop++;
            for(positive i = (*contStackTop)->containersCount; --i >= 0; ) {
                // push container
                transformation.pushMult((*contStackTop)->containerMatrices+i*MAT4_SCALARS_COUNT);
                *contStackTop = &(*contStackTop)->containers[i];
                contStackTop++;
            }
        }
    }
}

void SceneToPool::objectToPool(Object & object, Pool & pool) {
    poitive vertexOffset = pool.currentVerticesCount;
    mat4 matrix = transformation.getMatrix();

    // Per vertex
    vec4 vertices = pool.vertexPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec4 normals = pool.normalPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec4 colors = pool.colorPool + vertexOffset * VEC4_SCALARS_COUNT;
    vec2 mappings = pool.mappingPool + vertexOffset * VEC2_SCALARS_COUNT;
    positive * textures = pool.texturePool + vertexOffset;

    for(positive i = 0; i < object.verticesCount; i++) {
        // apply matrix to vertices and normals
        multiplyMV(matrix, object.vertices+i*VEC4_SCALARS_COUNT, vertices);
        multiplyMV(matrix, object.normals+i*VEC4_SCALARS_COUNT, normals);
        vertices += 4;
        normals += 4;

        // copy colors, mappings and textures
        memcpy(colors, object.colors+i*VEC4_SCALARS_COUNT, VEC4_SIZE);
        memcpy(mappings, object.mappings+i*VEC2_SCALARS_COUNT, VEC2_SIZE);
        textures = object.texture_id;
        colors += 4;
        mappings += 2;
        textures += 1;
    }

    pool.currentVerticesCount += object.verticesCount;


    // Per face
    positive * faces = pool.facePool + pool.currentFaceCount*3;

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
