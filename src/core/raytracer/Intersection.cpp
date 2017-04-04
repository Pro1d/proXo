#include "Intersection.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include <algorithm>
#include <limits>

#define EPSILON ((real) 0.000000001)

namespace proxo {

integer intersectTriangle(vec3 orig, vec3 dir, vec3 vert0, vec3 vert1,
    vec3 vert2, real* t, real* u, real* v)
{
	real edge1[VEC4_SCALARS_COUNT], edge2[VEC4_SCALARS_COUNT];
	real tvec[VEC4_SCALARS_COUNT], pvec[VEC4_SCALARS_COUNT],
	    qvec[VEC4_SCALARS_COUNT];
	real det, inv_det;
	integer side;

	substract(vert1, vert0, edge1);
	substract(vert2, vert0, edge2);

	cross(dir, edge2, pvec);

	det = dot(edge1, pvec);

	if(det > EPSILON)
		side = 1;
	else if(det < -EPSILON)
		side = -1;
	else /** face // ray */
		return 0;

	substract(orig, vert0, tvec);

	*u = dot(tvec, pvec);
	if(*u < 0 || *u > det)
		return 0;

	cross(tvec, edge1, qvec);

	*v = dot(dir, qvec);
	if(*v < 0 || *u + *v > det)
		return 0;

	*t      = dot(edge2, qvec);
	inv_det = 1 / det;
	*t *= inv_det;
	*u *= inv_det;
	*v *= inv_det;

	return side;
}

// paralSize = width / 2, height / 2, depth / 2
real intersectAxialParallelepiped(vec3 orig, vec3 inv_dir, real* paralMinMax)
{
	// ray is of form R + t D; assign min t as thit; normal N
	real tmin = 0, tmax = std::numeric_limits<real>::infinity();
	// intersect ray with x, y, z ``slabs'' (k = 0, 1, 2)
	for(int k = 0; k < 3; k++) {
		if(inv_dir[k] != std::numeric_limits<real>::infinity()) {
			real t1 =
			    (paralMinMax[k] - orig[k]) * inv_dir[k]; // plane x_k = -dx_k
			real t2 = (paralMinMax[k + BOUND_X_MAX] - orig[k])
			    * inv_dir[k]; // plane x_k = +dx_k
			tmin = std::max(tmin, std::min(t1, t2)); // intersect [tmin..
			tmax = std::min(tmax, std::max(t1, t2)); // tmax], [t1..t2]
			if(tmax <= tmin)
				return (real) -1; // no intersection
		}
		// ray parallel to plane
		else if(paralMinMax[k] > orig[k]
		    || orig[k] > paralMinMax[k + BOUND_X_MAX]) {
			return (real) -1; // no intersection
		}
	}

	return tmin;
}

void pushSubTree(vec3 orig, vec3 inv_dir, KDTree* tree, TreeStack& stack)
{
	real distFirst =
	    intersectAxialParallelepiped(orig, inv_dir, tree->firstSubTree->bounds);
	real distSecond = intersectAxialParallelepiped(
	    orig, inv_dir, tree->secondSubTree->bounds);
	bool intersectFirst  = distFirst >= 0;
	bool intersectSecond = distSecond >= 0;

	if(tree->middleSubTree != NULL) {
		real distMiddle = intersectAxialParallelepiped(
		    orig, inv_dir, tree->middleSubTree->bounds);
		bool intersectMiddle = distMiddle >= 0;

		if((distFirst < distSecond && intersectFirst) || !intersectSecond) {
			if((distMiddle < distFirst && intersectMiddle) || !intersectFirst) {
				if(intersectSecond)
					stack.push(tree->secondSubTree, distSecond);
				if(intersectFirst)
					stack.push(tree->firstSubTree, distFirst);
				if(intersectMiddle)
					stack.push(tree->middleSubTree, distMiddle);
			}
			else if((distMiddle < distSecond && intersectMiddle)
			    || !intersectSecond) {
				if(intersectSecond)
					stack.push(tree->secondSubTree, distSecond);
				if(intersectMiddle)
					stack.push(tree->middleSubTree, distMiddle);
				if(intersectFirst)
					stack.push(tree->firstSubTree, distFirst);
			}
			else {
				if(intersectMiddle)
					stack.push(tree->middleSubTree, distMiddle);
				if(intersectSecond)
					stack.push(tree->secondSubTree, distSecond);
				if(intersectFirst)
					stack.push(tree->firstSubTree, distFirst);
			}
		}
		else {
			if((distMiddle < distSecond && intersectMiddle)
			    || !intersectSecond) {
				if(intersectFirst)
					stack.push(tree->firstSubTree, distFirst);
				if(intersectSecond)
					stack.push(tree->secondSubTree, distSecond);
				if(intersectMiddle)
					stack.push(tree->middleSubTree, distMiddle);
			}
			else if((distMiddle < distFirst && intersectMiddle)
			    || !intersectFirst) {
				if(intersectFirst)
					stack.push(tree->firstSubTree, distFirst);
				if(intersectMiddle)
					stack.push(tree->middleSubTree, distMiddle);
				if(intersectSecond)
					stack.push(tree->secondSubTree, distSecond);
			}
			else {
				if(intersectMiddle)
					stack.push(tree->middleSubTree, distMiddle);
				if(intersectFirst)
					stack.push(tree->firstSubTree, distFirst);
				if(intersectSecond)
					stack.push(tree->secondSubTree, distSecond);
			}
		}
	}
	else {
		if((distFirst < distSecond && intersectFirst) || !intersectSecond) {
			if(intersectSecond)
				stack.push(tree->secondSubTree, distSecond);
			if(intersectFirst)
				stack.push(tree->firstSubTree, distFirst);
		}
		else {
			if(intersectFirst)
				stack.push(tree->firstSubTree, distFirst);
			if(intersectSecond)
				stack.push(tree->secondSubTree, distSecond);
		}
	}
}

void intersectSetOfTriangles(vec3 orig, vec3 dir, positive* faces,
    positive facesCount, vec4 vertices, positive* faceToIgnore,
    IntersectionData& out)
{
	positive* facesEnd = faces + facesCount * 4;
	for(positive* f = faces; f < facesEnd; f += 4) {
		real t, u, v;
		vec4 v1 = vertices + f[0] * VEC4_SCALARS_COUNT;
		vec4 v2 = vertices + f[1] * VEC4_SCALARS_COUNT;
		vec4 v3 = vertices + f[2] * VEC4_SCALARS_COUNT;
		integer intersect =
		    intersectTriangle(orig, dir, v1, v2, v3, &t, &u, &v);
		if(intersect != 0 && 0 < t && t < out.depth && f != faceToIgnore) {
			out.intersectionSide = intersect;
			out.depth            = t;
			out.uv[0]            = u;
			out.uv[1]            = v;
			out.face             = f;
		}
	}
}

void intersectSetOfTrianglesLighting(vec3 orig, vec3 dir, positive* faces,
    positive facesCount, vec4 vertices, vec16 materials, positive* faceToIgnore,
    SortedIntersectionsData& out)
{
	positive* facesEnd = faces + facesCount * 4;
	for(positive* f = faces; f < facesEnd; f += 4) {
		real t, u, v;
		vec4 v1 = vertices + f[0] * VEC4_SCALARS_COUNT;
		vec4 v2 = vertices + f[1] * VEC4_SCALARS_COUNT;
		vec4 v3 = vertices + f[2] * VEC4_SCALARS_COUNT;
		integer intersect =
		    intersectTriangle(orig, dir, v1, v2, v3, &t, &u, &v);
		if(intersect != 0 && 0 < t && f != faceToIgnore) {
			// Stop immediately if this is an opaque face
			real absorption = materials[f[0]*VEC16_SCALARS_COUNT+Pool::MAT_INDEX_ABSORPTION];
			if(absorption >= 1.0 - 1.0/255) {
				out.containsOpaqueFace = true;
				break;
			}
			// Otherwise register the face
			IntersectionData i;
			i.intersectionSide = intersect;
			i.depth            = t;
			i.uv[0]            = u;
			i.uv[1]            = v;
			i.face             = f;
			out.push(i);
		}
	}
}

void intersectTree(vec3 orig, vec3 dir, KDTree* root, TreeStack& stack,
    vec4 vertices, positive* faceToIgnore, IntersectionData& out)
{
	stack.clear();
	out.intersectionSide             = 0;
	out.depth                        = std::numeric_limits<real>::infinity();
	real inv_dir[VEC3_SCALARS_COUNT] = { 1 / dir[0], 1 / dir[1], 1 / dir[2] };

	real distRoot = intersectAxialParallelepiped(orig, inv_dir, root->bounds);
	if(distRoot < 0)
		return;
	stack.push(root, distRoot);

	while(!stack.empty()) {
		KDTree* node;
		real depth;
		stack.pop(node, depth);

		/// Test if this node can contain faces closer than the actual nearest
		/// face.
		if(depth < out.depth) {
			if(node->isLeaf) {
				intersectSetOfTriangles(orig, dir, node->faces,
				    node->facesCount, vertices, faceToIgnore, out);
			}
			else {
				pushSubTree(orig, inv_dir, node, stack);
			}
		}
	}
}

void intersectTreeLighting(vec3 orig, vec3 dir, KDTree* root, TreeStack& stack,
    vec4 vertices, vec16 materials, positive* faceToIgnore,
    SortedIntersectionsData& out)
{
	stack.clear();
	out.reset();
	real inv_dir[VEC3_SCALARS_COUNT] = { 1 / dir[0], 1 / dir[1], 1 / dir[2] };

	real distRoot = intersectAxialParallelepiped(orig, inv_dir, root->bounds);
	if(distRoot < 0)
		return;
	stack.push(root, distRoot);

	while(!stack.empty()) {
		KDTree* node;
		real depth;
		stack.pop(node, depth);

		if(node->isLeaf) {
			intersectSetOfTrianglesLighting(orig, dir, node->faces,
			    node->facesCount, vertices, materials, faceToIgnore, out);

			// End immediately if an opaque face has been found
			if(out.containsOpaqueFace)
				break;
		}
		else {
			pushSubTree(orig, inv_dir, node, stack);
		}
	}
}

} // namespace proxo
