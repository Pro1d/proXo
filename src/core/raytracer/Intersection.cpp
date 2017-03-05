#include "Intersection.h"
#include "core/math/Vector.h"
#include "core/math/type.h"
#include <algorithm>
#include <limits>

#define EPSILON ((real) 0.000000001)

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
real intersectAxialParallelepiped(
    vec4 orig, vec4 dir, vec3 paralMin, vec3 paralMax)
{
	// ray is of form R + t D; assign min t as thit; normal N
	real tmin = 0, tmax = std::numeric_limits<real>::infinity();
	// intersect ray with x, y, z ``slabs'' (k = 0, 1, 2)
	for(int k = 0; k < 3; k++) {
		if(dir[k] != 0) {
			real t1 = (paralMin[k] - orig[k]) / dir[k]; // plane x_k = -dx_k
			real t2 = (paralMax[k] - orig[k]) / dir[k]; // plane x_k = +dx_k
			tmin    = std::max(tmin, std::min(t1, t2)); // intersect [tmin..
			tmax    = std::min(tmax, std::max(t1, t2)); // tmax], [t1..t2]
			if(tmax <= tmin)
				return (real) -1; // no intersection
		}
		// ray parallel to plane
		else if(paralMin[k] > orig[k] || orig[k] > paralMax[k]) {
			return (real) -1; // no intersection
		}
	}

	return tmin;
}

void pushSubTree(vec3 orig, vec3 dir, KDTree* tree, TreeStack& stack)
{
	real distFirst = intersectAxialParallelepiped(orig, dir,
	    tree->firstSubTree->bounds + BOUND_X_MIN,
	    tree->firstSubTree->bounds + BOUND_X_MAX);
	real distSecond = intersectAxialParallelepiped(orig, dir,
	    tree->secondSubTree->bounds + BOUND_X_MIN,
	    tree->secondSubTree->bounds + BOUND_X_MAX);
	bool intersectFirst  = distFirst >= 0;
	bool intersectSecond = distSecond >= 0;

	if(tree->middleSubTree != NULL) {
		real distMiddle = intersectAxialParallelepiped(orig, dir,
		    tree->middleSubTree->bounds + BOUND_X_MIN,
		    tree->middleSubTree->bounds + BOUND_X_MAX);
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
	for(positive i = 0; i < facesCount; i++) {
		real t, u, v;
		vec4 v1 = vertices + faces[i * 4 + 0] * VEC4_SCALARS_COUNT;
		vec4 v2 = vertices + faces[i * 4 + 1] * VEC4_SCALARS_COUNT;
		vec4 v3 = vertices + faces[i * 4 + 2] * VEC4_SCALARS_COUNT;
		integer intersect =
		    intersectTriangle(orig, dir, v1, v2, v3, &t, &u, &v);
		if(intersect != 0 && 0 < t && t < out.depth
		    && faces + i * 4 != faceToIgnore) {
			out.intersectionSide = intersect;
			out.depth            = t;
			out.uv[0]            = u;
			out.uv[1]            = v;
			out.face             = faces + i * 4;
		}
	}
}

void intersectTree(vec3 orig, vec3 dir, KDTree* root, TreeStack& stack,
    vec4 vertices, positive* faceToIgnore, IntersectionData& out)
{
	KDTree** empty       = stack.treeStackTop;
	out.intersectionSide = 0;
	out.depth            = std::numeric_limits<real>::infinity();

	real distRoot = intersectAxialParallelepiped(
	    orig, dir, root->bounds, root->bounds + BOUND_X_MAX);
	if(distRoot < 0)
		return;
	stack.push(root, distRoot);

	while(stack.treeStackTop != empty) {
		KDTree* node;
		real depth;
		stack.pop(node, depth);

		/// Testi if this node can contain faces closer than the actual nearest
		/// face.
		if(depth < out.depth) {
			if(node->isLeaf) {
				intersectSetOfTriangles(orig, dir, node->faces,
				    node->facesCount, vertices, faceToIgnore, out);
			}
			else {
				pushSubTree(orig, dir, node, stack);
			}
		}
	}
}
