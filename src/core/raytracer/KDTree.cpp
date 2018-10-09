#include "KDTree.h"
#include "core/common/Pool.h"
#include "core/math/type.h"
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
  if(facesCount < 16)
    return false;
  const real COST_INTERSECTION = 2;
  const real COST_TRAVERSAL = 1;
  std::vector<real> primitive_start(facesCount);
  std::vector<real> primitive_end(facesCount);
  real cutCost = COST_INTERSECTION * facesCount;
  bool better_no_split = true;

  for(positive axis = 0; axis < 3; axis++) {
    // Build primitive start/end event lists
    const auto faces_end = facePtrs + facesCount;
    auto pStart = primitive_start.begin();
    auto pEnd = primitive_end.begin();
    for(auto face = facePtrs; face != faces_end; ++face, ++pStart, ++pEnd) {
      const auto a = pool.vertexPool[(*face)[0] * VEC4_SCALARS_COUNT + axis];
      const auto b = pool.vertexPool[(*face)[1] * VEC4_SCALARS_COUNT + axis];
      const auto c = pool.vertexPool[(*face)[2] * VEC4_SCALARS_COUNT + axis];
      *pStart = std::min({a,b,c});
      *pEnd = std::max({a,b,c});
    }
    // Sort event lists
    std::sort(primitive_start.begin(), primitive_start.end());
    std::sort(primitive_end.begin(), primitive_end.end());
    
    // iterate over events to find best split
    pStart = primitive_start.begin();
    pEnd = primitive_end.begin();
    positive leftCount = 0;
    positive rightCount = facesCount;
    const real inv_size = 1 / (bounds[axis + 3] - bounds[axis]);
    while(pStart != primitive_start.end() || pEnd != primitive_end.end()) {
      real cut;
      if(pEnd == primitive_end.end() || (pStart != primitive_start.end() && *pStart < *pEnd)) {
        cut = *pStart;
        ++leftCount;
        ++pStart;
      } else {
        cut = *pEnd;
        --rightCount;
        ++pEnd;
      }
      if(bounds[axis] < cut && cut < bounds[axis + 3]) {
        // Get cost for current cut
        real k = (cut - bounds[axis]) * inv_size;
        real cost = COST_TRAVERSAL + COST_INTERSECTION * (k * leftCount + (1-k) * rightCount);
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
    positive** leftFacePtrs = facePtrs;
    positive** rightFacePtrs = new positive*[cutFaceCounts[1]];
    auto lfp_it = leftFacePtrs;
    auto rfp_it = rightFacePtrs;
    for(positive i = 0; i < facesCount; i++) {
      const auto face = facePtrs[i];
      if(pool.vertexPool[face[0] * VEC4_SCALARS_COUNT + cutAxis] < cutValue ||
          pool.vertexPool[face[1] * VEC4_SCALARS_COUNT + cutAxis] < cutValue ||
          pool.vertexPool[face[2] * VEC4_SCALARS_COUNT + cutAxis] < cutValue) {
        *lfp_it = face;
        ++lfp_it;
      }
      if(pool.vertexPool[face[0] * VEC4_SCALARS_COUNT + cutAxis] > cutValue ||
          pool.vertexPool[face[1] * VEC4_SCALARS_COUNT + cutAxis] > cutValue ||
          pool.vertexPool[face[2] * VEC4_SCALARS_COUNT + cutAxis] > cutValue) {
        *rfp_it = face;
        ++rfp_it;
      }
    }
    auto* branch = new Branch(cutAxis, cutValue);
    node = branch;
    real min = bounds[cutAxis];
    real max = bounds[cutAxis + 3];
    
    bounds[cutAxis + 3] = cutValue;
    buildNode(branch->left, bounds, pool, leftFacePtrs, lfp_it - leftFacePtrs, d+1);
    bounds[cutAxis + 3] = max;

    bounds[cutAxis] = cutValue;
    buildNode(branch->right, bounds, pool, rightFacePtrs, rfp_it - rightFacePtrs, d+1);
    bounds[cutAxis] = min;
    
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
