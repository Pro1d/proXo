#include "KDTree.h"
#include "core/common/Pool.h"
#include "core/math/type.h"
#include "Intersection.h"
#include <algorithm>
#include <stack>
#include <utility>
#include <cstdio>
#include <cstring>
#include <vector>

namespace proxo {

KDTree::KDTree(const Pool& pool)
{
  positive** facePtrs = new positive*[pool.currentFacesCount];
  for(positive i = 0; i < pool.currentFacesCount; i++) {
    facePtrs[i] = pool.facePool + i * 4;
  }

  setBounds(pool);
  buildNode(root, bounds, pool, facePtrs, pool.currentFacesCount);

  delete facePtrs;

  updateDepthMax();
}

KDTree::~KDTree()
{
  if(root)
    delete root;
}

void KDTree::setBounds(const Pool& pool)
{
	bounds[BOUND_X_MIN] = pool.vertexPool[pool.facePool[0] * VEC4_SCALARS_COUNT + 0];
	bounds[BOUND_Y_MIN] = pool.vertexPool[pool.facePool[0] * VEC4_SCALARS_COUNT + 1];
	bounds[BOUND_Z_MIN] = pool.vertexPool[pool.facePool[0] * VEC4_SCALARS_COUNT + 2];
	bounds[BOUND_X_MAX] = pool.vertexPool[pool.facePool[0] * VEC4_SCALARS_COUNT + 0];
	bounds[BOUND_Y_MAX] = pool.vertexPool[pool.facePool[0] * VEC4_SCALARS_COUNT + 1];
	bounds[BOUND_Z_MAX] = pool.vertexPool[pool.facePool[0] * VEC4_SCALARS_COUNT + 2];

	for(positive i = 0; i < pool.currentFacesCount; i++) {
		for(positive j = 0; j < 3; j++) {
			positive idx = pool.facePool[i * 4 + j] * VEC4_SCALARS_COUNT;
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

bool KDTree::chooseCut(const Pool& pool, real bounds[6], positive** facePtrs, positive facesCount, positive& cutAxis, real& cutValue, positive cutFaceCounts[2])
{
  const positive cutTestCount = 16;
  const real COST_INTERSECTION = 2;
  const real COST_TRAVERSAL = 1;
  const auto faces_end = facePtrs + facesCount;
  real cutCost = COST_INTERSECTION * facesCount;
  bool better_no_split = true;

  real size_x = bounds[3] - bounds[0];
  real size_y = bounds[4] - bounds[1];
  real size_z = bounds[5] - bounds[2];
  real m = std::max({size_x, size_y, size_z});
  bool valid_axis[3] = {
    (size_x > m * 0.3),
    (size_y > m * 0.3),
    (size_z > m * 0.3),
  };
  for(positive axis = 0; axis < 3; axis++)
  if(valid_axis[axis]) {
    real leftBounds[6]; memcpy(leftBounds, bounds, sizeof(real)*6);
    real rightBounds[6]; memcpy(rightBounds, bounds, sizeof(real)*6);
    for(positive cutIndex = 1; cutIndex <= cutTestCount; cutIndex++) {
      auto cutRatio = (real) cutIndex / (cutTestCount + 1);
      auto cut = cutRatio * bounds[axis+3] + (1 - cutRatio) * bounds[axis];
      leftBounds[axis + 3] = cut;
      rightBounds[axis] = cut;

      // Build primitive start/end event lists
      positive leftCount = 0;
      positive rightCount = 0;
      for(auto face = facePtrs; face != faces_end; ++face) {
        const auto a = pool.vertexPool + (*face)[0] * VEC4_SCALARS_COUNT;
        const auto b = pool.vertexPool + (*face)[1] * VEC4_SCALARS_COUNT;
        const auto c = pool.vertexPool + (*face)[2] * VEC4_SCALARS_COUNT;
        bool in_left = false, in_right = false;
        if(a[axis] < cut || b[axis] < cut || c[axis] < cut)
        if(intersectionAABBTriangle(leftBounds, a, b, c))
        {
          leftCount++;
          in_left = true;
        }
        if(a[axis] > cut || b[axis] > cut || c[axis] > cut)
        if(intersectionAABBTriangle(rightBounds, a, b, c))
        {
          rightCount++;
          in_right = true;
        }
        if (not in_right and not in_left) {
          printf("Not in any side: %d triagles, %f %f %f > %d %f\n", facesCount, size_x, size_y, size_z, axis, cutRatio);
          rightCount = leftCount = facesCount * 2;
        }
      }

      // Get cost for current cut
      real cost = COST_TRAVERSAL + COST_INTERSECTION * (cutRatio * leftCount + (1-cutRatio) * rightCount) * m / (bounds[axis+3] - bounds[axis]);
      if(rightCount == 0 || leftCount == 0)
        cost *= 0.8;
      if(cost < cutCost) {
        cutCost = cost;
        cutAxis = axis;
        cutValue = cut;
        cutFaceCounts[0] = leftCount;
        cutFaceCounts[1] = rightCount;
        better_no_split = false;
      }
    }
  }
  return not better_no_split;
}

void KDTree::buildNode(Node*& node, real bounds[6], const Pool& pool, positive** facePtrs, positive facesCount, positive d)
{
  positive cutAxis;
  real cutValue;
  positive cutFaceCounts[2];
  bool validCut = chooseCut(pool, bounds, facePtrs, facesCount, cutAxis, cutValue, cutFaceCounts);
  if(validCut) {
    real left_bounds[6], right_bounds[6];
    memcpy(left_bounds, bounds, sizeof(float)*6);
    memcpy(right_bounds, bounds, sizeof(float)*6);
    left_bounds[cutAxis + 3] = cutValue;
    right_bounds[cutAxis] = cutValue;

    positive** leftFacePtrs = facePtrs;
    positive** rightFacePtrs = new positive*[cutFaceCounts[1]];
    auto lfp_it = leftFacePtrs;
    auto rfp_it = rightFacePtrs;
    for(positive i = 0; i < facesCount; i++) {
      const auto face = facePtrs[i];
      vec3 a = pool.vertexPool + face[0] * VEC4_SCALARS_COUNT;
      vec3 b = pool.vertexPool + face[1] * VEC4_SCALARS_COUNT;
      vec3 c = pool.vertexPool + face[2] * VEC4_SCALARS_COUNT;
      if(a[cutAxis] < cutValue || b[cutAxis] < cutValue || c[cutAxis] < cutValue) {
        if(intersectionAABBTriangle(left_bounds, a,b,c)) {
          *lfp_it = face;
          ++lfp_it;
        }
      }
      if(a[cutAxis] > cutValue || b[cutAxis] > cutValue || c[cutAxis] > cutValue) {
        if(intersectionAABBTriangle(right_bounds, a,b,c)) {
          *rfp_it = face;
          ++rfp_it;
        }
      }
    }
    auto* branch = new Branch(cutAxis, cutValue);
    node = branch;
    
    buildNode(branch->left, left_bounds, pool, leftFacePtrs, lfp_it - leftFacePtrs, d+1);
    buildNode(branch->right, right_bounds, pool, rightFacePtrs, rfp_it - rightFacePtrs, d+1);
    
    delete rightFacePtrs;
  }
  else {
    auto* leaf = new Leaf(facesCount);
    node = leaf;
    std::memcpy(leaf->facePtrs, facePtrs, sizeof(positive*) * facesCount);
  }
}

positive KDTree::updateDepthMax() {
  std::stack<std::pair<positive, Node*>> s;
  s.push({1, root});
  positive max = 0;
  while(!s.empty()) {
    auto n = s.top();
    s.pop();
    if(n.first > max)
      max = n.first;
    if(!n.second->is_leaf) {
      s.push({n.first+1, static_cast<Branch*>(n.second)->left});
      s.push({n.first+1, static_cast<Branch*>(n.second)->right});
    }
  }
  depthMax = max;
  return max;
}

} // namespace proxo
