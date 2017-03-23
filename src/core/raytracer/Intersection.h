#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include "KDTree.h"
#include "core/math/type.h"

namespace proxo {

class IntersectionData {
public:
	positive* face;
	real uv[2];
	real depth;
	integer intersectionSide; // -1 culling, 0 no intersect, 1 intersect
};

integer intersectTriangle(vec3 orig, vec3 dir, vec3 vert0, vec3 vert1,
    vec3 vert2, real* t, real* u, real* v);
real intersectAxialParallelepiped(
    vec3 orig, vec3 inv_dir, real* paralMinMax);
void pushSubTree(vec3 orig, vec3 inv_dir, KDTree* tree, TreeStack& stack);
void intersectSetOfTriangles(vec3 orig, vec3 inv_dir, positive* faces,
    positive facesCount, vec4 vertices, positive* faceToIgnore,
    IntersectionData& out);
void intersectTree(vec3 orig, vec3 dir, KDTree* tree, TreeStack& stack,
    vec4 vertices, positive* faceToIgnore, IntersectionData& out);

} // namespace proxo

#endif // __INTERSECTION_H__
