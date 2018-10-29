#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include "KDTree.h"
#include "core/math/type.h"
#include <algorithm>
#include <limits>
#include <vector>

namespace proxo {

struct IntersectionData {
	positive* face;
	real uv[2];
	real depth;
	integer intersectionSide; // -1 culling, 0 no intersect, 1 intersect
	bool operator<(IntersectionData const& b) const { return depth > b.depth; } // TODO with current tree traversal method, intersection are more likely in ascending order of depth. maybe...
};

class SortedIntersectionsData : public std::vector<IntersectionData> {
public:
	void reset()
	{
		clear();
		containsOpaqueFace = false;
	}
  // unique and sort, must be called when completely filled
  void finalize() {
    unique();
    sort(); // as faces are globally already sorted, might faster to use priority and not have to check for unique() elements
  }
	bool containsOpaqueFace;
private:
  void unique()
  {
    std::sort(begin(), end(), [](auto&& a, auto&& b) { return a.face < b.face; });
    auto last = std::unique(begin(), end(), [](auto&& a, auto&& b) { return a.face == b.face; });
    erase(last, end());
  }
  // you must call unique() before calling sort()
  void sort()
  {
    std::sort(begin(), end());
  }
};

integer intersectTriangle(vec3 orig, vec3 dir, vec3 vert0, vec3 vert1,
    vec3 vert2, real* t, real* u, real* v);
bool intersectAxialParallelepiped(const vec3 orig, const vec3 inv_dir, const real* paralMinMax, real& tmin, real& tmax);
void intersectSetOfTriangles(vec3 orig, vec3 inv_dir, positive** faces,
    positive facesCount, vec4 vertices, positive* faceToIgnore, real dmax,
    IntersectionData& out);
void intersectSetOfTrianglesLighting(vec3 orig, vec3 dir, positive** faces,
    positive facesCount, vec4 vertices, vec16 materials, positive* faceToIgnore,
    real distanceMax, SortedIntersectionsData& out);
bool intersectKDTree(NodeStack& stack, KDTree& tree, vec4 vertices,
    positive* faceToIgnore, vec3 ray_orig, vec3 ray_dir, real dmax, IntersectionData& out);
bool intersectKDTreeLighting(NodeStack& stack, KDTree& tree, vec4 vertices, vec16 materals,
    positive* faceToIgnore, vec3 ray_orig, vec3 ray_dir, real dmax, SortedIntersectionsData& out);
bool intersectionAABBTriangle(real AABB[6], vec3 a, vec3 b, vec3 c);

} // namespace proxo

#endif // __INTERSECTION_H__
