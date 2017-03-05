#include "KDTree.h"
#include "core/common/Pool.h"
#include "core/math/type.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

KDTree::KDTree(Pool& pool, positive* faces, positive faceBegin,
    positive faceEnd, KDTree* parent)
    : middleSubTree(NULL), firstSubTree(NULL), secondSubTree(NULL),
      isLeaf(false), facesCount(faceEnd - faceBegin),
      faces(faces + faceBegin * 4)
{
	if(parent == NULL) {
		isLocked[0] = false;
		isLocked[1] = false;
		isLocked[2] = false;
	}
	else {
		isLocked[0] = parent->isLocked[0];
		isLocked[1] = parent->isLocked[1];
		isLocked[2] = parent->isLocked[2];
	}
}

KDTree::~KDTree()
{
	if(middleSubTree != NULL)
		delete middleSubTree;
	if(firstSubTree != NULL)
		delete firstSubTree;
	if(secondSubTree != NULL)
		delete secondSubTree;
}

void KDTree::setBounds(Pool& pool)
{
	bounds[BOUND_X_MIN] = pool.vertexPool[faces[0] * VEC4_SCALARS_COUNT + 0];
	bounds[BOUND_Y_MIN] = pool.vertexPool[faces[0] * VEC4_SCALARS_COUNT + 1];
	bounds[BOUND_Z_MIN] = pool.vertexPool[faces[0] * VEC4_SCALARS_COUNT + 2];
	bounds[BOUND_X_MAX] = pool.vertexPool[faces[0] * VEC4_SCALARS_COUNT + 0];
	bounds[BOUND_Y_MAX] = pool.vertexPool[faces[0] * VEC4_SCALARS_COUNT + 1];
	bounds[BOUND_Z_MAX] = pool.vertexPool[faces[0] * VEC4_SCALARS_COUNT + 2];

	for(positive i = 0; i < facesCount; i++) {
		for(positive j = 0; j < 3; j++) {
			positive idx = faces[i * 4 + j] * VEC4_SCALARS_COUNT;
			bounds[BOUND_X_MIN] =
			    std::min(bounds[BOUND_X_MIN], pool.vertexPool[idx + 0]);
			bounds[BOUND_Y_MIN] =
			    std::min(bounds[BOUND_Y_MIN], pool.vertexPool[idx + 1]);
			bounds[BOUND_Z_MIN] =
			    std::min(bounds[BOUND_Z_MIN], pool.vertexPool[idx + 2]);
			bounds[BOUND_X_MAX] =
			    std::max(bounds[BOUND_X_MAX], pool.vertexPool[idx + 0]);
			bounds[BOUND_Y_MAX] =
			    std::max(bounds[BOUND_Y_MAX], pool.vertexPool[idx + 1]);
			bounds[BOUND_Z_MAX] =
			    std::max(bounds[BOUND_Z_MAX], pool.vertexPool[idx + 2]);
		}
	}
}

void KDTree::build(Pool& pool)
{
	setBounds(pool);
	real size[3] = { bounds[BOUND_X_MAX] - bounds[BOUND_X_MIN],
		bounds[BOUND_Y_MAX] - bounds[BOUND_Y_MIN],
		bounds[BOUND_Z_MAX] - bounds[BOUND_Z_MIN] };

	if(facesCount < MINIMUM_FACES_TREE
	    || (isLocked[0] && isLocked[1] && isLocked[2])) {
		isLeaf = true;
		return;
	}

	// Choose cut axis

	positive cutAxis = 0;
	if(size[1] > size[0] || isLocked[0])
		cutAxis = 1;
	if((size[2] > size[1] || isLocked[1]) && (size[2] > size[0] || isLocked[0]))
		cutAxis = 2;

	// choose cut value
	real cutValue = size[cutAxis] / 2 + bounds[cutAxis];

	// separate faces in second side from faces in first/middle side
	positive firstBegin = 0, firstEnd = facesCount;
	positive secondBegin = facesCount, secondEnd = facesCount;
	for(positive i = 0; i < secondBegin; i++) {
		positive idx = i * 4;
		// if the face is in second side
		if(pool.vertexPool[faces[idx + 0] * VEC4_SCALARS_COUNT + cutAxis]
		        >= cutValue
		    && pool.vertexPool[faces[idx + 1] * VEC4_SCALARS_COUNT + cutAxis]
		        >= cutValue
		    && pool.vertexPool[faces[idx + 2] * VEC4_SCALARS_COUNT + cutAxis]
		        >= cutValue) {
			// swap face[i] and face[secondBegin-1]
			secondBegin--;
			positive face[4];
			memcpy(face, faces + idx, sizeof(positive) * 4);
			memcpy(faces + idx, faces + secondBegin * 4, sizeof(positive) * 4);
			memcpy(faces + secondBegin * 4, face, sizeof(positive) * 4);
			i--;
		}
	}
	firstEnd = secondBegin;

	// separate faces in middle side from faces in first side
	positive middleBegin = firstEnd, middleEnd = secondBegin;
	for(positive i = 0; i < middleBegin; i++) {
		positive idx = i * 4;
		// if the face is in middle side
		if(pool.vertexPool[faces[idx + 0] * VEC4_SCALARS_COUNT + cutAxis]
		        > cutValue
		    || pool.vertexPool[faces[idx + 1] * VEC4_SCALARS_COUNT + cutAxis]
		        > cutValue
		    || pool.vertexPool[faces[idx + 2] * VEC4_SCALARS_COUNT + cutAxis]
		        > cutValue) {
			// swap face[i] and face[middleBegin-1]
			middleBegin--;
			positive face[4];
			memcpy(face, faces + idx, sizeof(positive) * 4);
			memcpy(faces + idx, faces + middleBegin * 4, sizeof(positive) * 4);
			memcpy(faces + middleBegin * 4, face, sizeof(positive) * 4);
			i--;
		}
	}
	firstEnd = middleBegin;

	// recc build
	bool isWiselySplitted = firstBegin < firstEnd && secondBegin < secondEnd;
	if(isWiselySplitted) {
		if(middleBegin < middleEnd) {
			middleSubTree =
			    new KDTree(pool, faces, middleBegin, middleEnd, this);
			middleSubTree->isLocked[cutAxis] = true;
			middleSubTree->build(pool);
		}
		if(firstBegin < firstEnd) {
			firstSubTree = new KDTree(pool, faces, firstBegin, firstEnd, this);
			firstSubTree->build(pool);
		}
		if(secondBegin < secondEnd) {
			secondSubTree =
			    new KDTree(pool, faces, secondBegin, secondEnd, this);
			secondSubTree->build(pool);
		}
	}
	else {
		isLeaf = true;
	}
}

void KDTree::print(positive depth)
{
	for(positive i = 0; i < depth; i++)
		printf("\t");
	printf("%f %f %f %f %f %f | %d | %d %d %d\n", bounds[0], bounds[1],
	    bounds[2], bounds[3], bounds[4], bounds[5], facesCount, isLocked[0],
	    isLocked[1], isLocked[2]);

	if(firstSubTree != NULL) {
		for(positive i = 0; i <= depth; i++)
			printf("\t");
		printf("firstSubTree\n");
		firstSubTree->print(depth + 1);
	}
	if(secondSubTree != NULL) {
		for(positive i = 0; i <= depth; i++)
			printf("\t");
		printf("secondSubTree\n");
		secondSubTree->print(depth + 1);
	}
	if(middleSubTree != NULL) {
		for(positive i = 0; i <= depth; i++)
			printf("\t");
		printf("middleSubTree\n");
		middleSubTree->print(depth + 1);
	}
}
