#include "KDTreeOld.h"
#include "core/common/Pool.h"
#include "core/math/type.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace proxo {

KDTreeOld::KDTreeOld(Pool& pool, positive* faces, positive faceBegin,
    positive faceEnd, KDTreeOld* parent)
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

KDTreeOld::~KDTreeOld()
{
	if(middleSubTree != NULL)
		delete middleSubTree;
	if(firstSubTree != NULL)
		delete firstSubTree;
	if(secondSubTree != NULL)
		delete secondSubTree;
}

void KDTreeOld::setBounds(Pool& pool)
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

void KDTreeOld::chooseCut(Pool& pool, positive& cutAxis, real& cutValue)
{
  const positive cutTestCount = 512;
	real size[3] = {
    bounds[BOUND_X_MAX] - bounds[BOUND_X_MIN],
		bounds[BOUND_Y_MAX] - bounds[BOUND_Y_MIN],
		bounds[BOUND_Z_MAX] - bounds[BOUND_Z_MIN] };

	// Choose cut axis
	cutAxis = 0;
	if((size[1] > size[0] || isLocked[0]) && !isLocked[1])
		cutAxis = 1;
	if((size[2] > size[1] || isLocked[1]) && (size[2] > size[0] || isLocked[0]) && !isLocked[2])
		cutAxis = 2;
  
  positive left[3*cutTestCount]; // Face count completely left to cut
  positive right[3*cutTestCount]; // Face count completely right to cut
  memset(left, 0, sizeof(positive)*3*cutTestCount);
  memset(right, 0, sizeof(positive)*3*cutTestCount);
  for(positive i = 0; i < facesCount; i++) {
    for(positive axis = 0; axis < 3; axis++) {
      if(!isLocked[axis]) {
        real a = pool.vertexPool[faces[i*4+0]*VEC4_SCALARS_COUNT + axis];
        real b = pool.vertexPool[faces[i*4+1]*VEC4_SCALARS_COUNT + axis];
        real c = pool.vertexPool[faces[i*4+2]*VEC4_SCALARS_COUNT + axis];
        real maxLeft = std::max(std::max(a, b), c);
        real minRight = std::min(std::min(a, b), c);
        real ratioLeft = (maxLeft - bounds[axis]) / size[axis];
        real ratioRight = (minRight - bounds[axis]) / size[axis];
        integer cutIndexLeft = ratioLeft * (cutTestCount+1);
        integer cutIndexRight = ratioRight * (cutTestCount+1);
        if(cutIndexLeft < cutTestCount)
          left[cutIndexLeft + cutTestCount * axis]++;
        if(cutIndexRight > 0)
          right[cutIndexRight-1 + cutTestCount * axis]++;
      }
    }
  }
  for(positive cutIndex = 1; cutIndex < cutTestCount; cutIndex++) {
    for(positive axis = 0; axis < 3; axis++) {
      if(!isLocked[axis]) {
        left[cutIndex + cutTestCount * axis] += left[cutIndex-1 + cutTestCount * axis];
        right[(cutTestCount - cutIndex - 1) + cutTestCount * axis] += right[(cutTestCount - cutIndex) + cutTestCount * axis];
      }
    }
  }
  //cutAxis = 0;
  positive cutIndex = 0;
  real cutCost = -1;
  for(positive axis = 0; axis < 3; axis++) {
    if(!isLocked[axis] && axis == cutAxis) {
      for(positive cut = 0; cut < cutTestCount; cut++) {
        real cost = ((cut+1) * left[cut + cutTestCount * axis]
          + (cutTestCount-cut) * right[cut + cutTestCount * axis]
          + (cutTestCount+1) *  (facesCount - (left[cut + cutTestCount * axis] + right[cut + cutTestCount * axis])));// / size[axis];
        if(cutCost < 0 or cost < cutCost) {
          cutIndex = cut;
          cutAxis = axis;
          cutCost = cost;
        }
      }
    }
  }
  cutValue = bounds[cutAxis] + size[cutAxis] * (cutIndex+1) / (cutTestCount+1);

  /**
   * cut test with cost function
   *
  positive sum[3*(cutTestCount+1)];
  memset(sum, 0, sizeof(positive)*3*(cutTestCount+1));
  for(positive i = 0; i < facesCount; i++) {
    for(positive j = 0; j < 3; j++) {
      for(positive axis = 0; axis < 3; axis++) {
        if(!isLocked[axis]) {
          real a = pool.vertexPool[faces[i*4+j]*VEC4_SCALARS_COUNT + axis];
          real ratio = (a - bounds[axis]) / size[axis];
          integer cutIndex = ratio * (cutTestCount+1);
          sum[cutIndex + (cutTestCount+1) * axis]++;
        }
      }
    }
  }
  for(positive cutIndex = 1; cutIndex < cutTestCount+1; cutIndex++) {
    for(positive axis = 0; axis < 3; axis++) {
      sum[cutIndex + (cutTestCount+1) * axis] += sum[cutIndex-1 + (cutTestCount+1) * axis];
    }
  }
  cutAxis = 0;
  positive cutIndex = 0;
  positive cutCost = -1;
  for(positive axis = 0; axis < 3; axis++) {
    if(!isLocked[axis]) {
      for(positive cut = 0; cut < cutTestCount; cut++) {
        positive cost = (cut+1) * sum[cut + (cutTestCount+1) * axis]
          + (cutTestCount-cut) * (sum[cutTestCount + (cutTestCount+1) * axis] - sum[cut + (cutTestCount+1) * axis]);
        if(cost < cutCost) {
          cutIndex = cut;
          cutAxis = axis;
          cutCost = cost;
        }
      }
    }
  }
  cutValue = bounds[cutAxis] + size[cutAxis] * (cutIndex+1) / (cutTestCount+1);

  /**
   * Mean
   *
	// Choose cut axis
	cutAxis = 0;
	if((size[1] > size[0] || isLocked[0]) && !isLocked[1])
		cutAxis = 1;
	if((size[2] > size[1] || isLocked[1]) && (size[2] > size[0] || isLocked[0]) && !isLocked[2])
		cutAxis = 2;

	// choose cut value
	real mean = 0;
	for(positive i = 0; i < facesCount; i++) {
		mean += pool.vertexPool[faces[i*4+0]*VEC4_SCALARS_COUNT+cutAxis];
		mean += pool.vertexPool[faces[i*4+1]*VEC4_SCALARS_COUNT+cutAxis];
		mean += pool.vertexPool[faces[i*4+2]*VEC4_SCALARS_COUNT+cutAxis];
	}
	cutValue = mean / (3*facesCount);
  */
}

void KDTreeOld::build(Pool& pool)
{
	setBounds(pool);

	if(facesCount < MINIMUM_FACES_TREE
	    || (isLocked[0] && isLocked[1] && isLocked[2])) {
		isLeaf = true;
		return;
	}

  positive cutAxis;
  real cutValue;
  chooseCut(pool, cutAxis, cutValue);

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
			    new KDTreeOld(pool, faces, middleBegin, middleEnd, this);
			middleSubTree->isLocked[cutAxis] = true;
			middleSubTree->build(pool);
		}
		if(firstBegin < firstEnd) {
			firstSubTree = new KDTreeOld(pool, faces, firstBegin, firstEnd, this);
			firstSubTree->build(pool);
		}
		if(secondBegin < secondEnd) {
			secondSubTree =
			    new KDTreeOld(pool, faces, secondBegin, secondEnd, this);
			secondSubTree->build(pool);
		}
	}
	else {
		isLocked[cutAxis] = true;
		build(pool);
	}
}

void KDTreeOld::print(positive depth)
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

} // namespace proxo
