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

	inv_det = 1 / det;
	substract(orig, vert0, tvec);

	*u = dot(tvec, pvec) * inv_det;
	if(*u < 0 || *u > 1)
		return 0;

	cross(tvec, edge1, qvec);

	*v = dot(dir, qvec) * inv_det;
	if(*v < 0 || *u + *v > 1)
		return 0;

	*t = dot(edge2, qvec) * inv_det;

	return side;
}

bool intersectAxialParallelepiped(const vec3 orig, const vec3 inv_dir, const real* paralMinMax, real& tmin, real& tmax)
{
	// ray is of form R + t D; assign min t as thit; normal N
	tmin = 0;
  tmax = std::numeric_limits<real>::infinity();
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
				return false; // no intersection
		}
		// ray parallel to plane
		else if(paralMinMax[k] > orig[k]
		    || orig[k] > paralMinMax[k + BOUND_X_MAX]) {
			return false; // no intersection
		}
	}

	return true;
}

void intersectSetOfTriangles(vec3 orig, vec3 dir, positive** faces,
    positive facesCount, vec4 vertices, positive* faceToIgnore,
    IntersectionData& out)
{
	auto facesEnd = faces + facesCount;
	for(auto it = faces; it != facesEnd; it++) {
    const auto f = *it;
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

void intersectSetOfTrianglesLighting(vec3 orig, vec3 dir, positive** faces,
    positive facesCount, vec4 vertices, vec16 materials, positive* faceToIgnore,
    real distanceMax, SortedIntersectionsData& out)
{
	auto facesEnd = faces + facesCount;
	for(auto it = faces; it != facesEnd; it++) {
    const auto f = *it;
		real t, u, v;
		vec4 v1 = vertices + f[0] * VEC4_SCALARS_COUNT;
		vec4 v2 = vertices + f[1] * VEC4_SCALARS_COUNT;
		vec4 v3 = vertices + f[2] * VEC4_SCALARS_COUNT;
		integer intersect =
		    intersectTriangle(orig, dir, v1, v2, v3, &t, &u, &v);
		if(intersect != 0 && 0 < t && f != faceToIgnore && t < distanceMax) {
			// Stop immediately if this is an opaque face
			vec3 absorption = materials + f[0] * VEC16_SCALARS_COUNT
			    + Pool::MAT_INDEX_ABSORPTION_RED;
			bool isOpaque = absorption[0] > 1 - 1.0 / (255*255)
			    && absorption[1] > 1 - 1.0 / (255*255)
			    && absorption[2] > 1 - 1.0 / (255*255); // TODO optim: store isOpaque in 'materials'
			if(isOpaque) {
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
			out.push_back(i); // are we sure it does not reallocate the *data?
		}
	}
}

/*
  intersection = none;
  if (ray intersects root node) {
      stack.push(root node, tmin, tmax); // stack size max = tree depth Max +/-1
      while (!stack.empty() && !intersection) {
          (node, tmin, tmax) = stack.pop();
          while (!node.isLeaf()) {
              tsplit = (node.split - ray.origin[node.axis]) / ray.direction[node.axis];
              if (node.split - ray.origin[node.axis] >= 0) {
                  first = node.left;
                  second = node.right;
              } else {
                  first = node.right;
                  second = node.left;
              }
              if (tsplit >= tmax || tsplit < 0)
                  node = first; // update tmin or tmax ? maybe no need in that case. No need!
              else if (tsplit <= tmin)
                  node = second; // update tmin or tmax ? maybe no need in that case. No need!
              else {
                  stack.push(second, tsplit, tmax);
                  node = first;
                  tmax = tsplit;
              }
          }
          foreach (triangle in node) // optim to do add triangle to leaf if not intersect AABB
              if (ray intersects triangle)
                  intersection = nearest intersection (in range [tmin; tmax]);
          if (nearest intersection > tmax) ("in range [tmin; tmax]" -> done here!)
                  intersection = none; // optim aformentioned could avoid this case
      }
  }
*/
bool intersectKDTree(NodeStack& stack, KDTree& tree, vec4 vertices, positive* faceToIgnore, vec3 ray_orig, vec3 ray_dir, real dmax, IntersectionData& out)
{
	out.intersectionSide = 0;
	out.depth = dmax; //std::numeric_limits<real>::infinity();
  real ray_inv_dir[VEC3_SCALARS_COUNT] = {
    1 / ray_dir[0],
    1 / ray_dir[1],
    1 / ray_dir[2]
  };
  real tmin_root, tmax_root;
  bool inter_root = intersectAxialParallelepiped(ray_orig, ray_inv_dir, tree.bounds, tmin_root, tmax_root);
  if(not inter_root || dmax <= tmin_root)
    return false;

  stack.clear();
  stack.push({tree.root, tmin_root, std::min(tmax_root, dmax)});

  while(not stack.empty() && out.intersectionSide == 0) {
    auto e = stack.pop();
    
    while(not e.node->is_leaf) {
      auto branch = static_cast<Branch*>(e.node);
      real rel = branch->cutValue - ray_orig[branch->cutAxis];
      real tsplit = rel * ray_inv_dir[branch->cutAxis];
      Node* first;
      Node* second;
      if(rel >= 0) {
        first = branch->left;
        second = branch->right;
      }
      else {
        first = branch->right;
        second = branch->left;
      }
      if(tsplit >= e.tmax || tsplit < 0)
        e.node = first;
      else if (tsplit <= e.tmin)
        e.node = second;
      else {
        stack.push({second, tsplit, e.tmax});
        e.node = first;
        e.tmax = tsplit;
      }
    }

    auto leaf = static_cast<Leaf*>(e.node);
    intersectSetOfTriangles(ray_orig, ray_dir, leaf->facePtrs,
      leaf->facesCount, vertices, faceToIgnore, out);
  }

  return out.intersectionSide != 0;
}

bool intersectKDTreeLighting(NodeStack& stack, KDTree& tree, vec4 vertices, vec16 materials, positive* faceToIgnore, vec3 ray_orig, vec3 ray_dir, real dmax, SortedIntersectionsData& out)
{
  out.reset();
  real ray_inv_dir[VEC3_SCALARS_COUNT] = {
    1 / ray_dir[0],
    1 / ray_dir[1],
    1 / ray_dir[2]
  };
  real tmin_root, tmax_root;
  bool inter_root = intersectAxialParallelepiped(ray_orig, ray_inv_dir, tree.bounds, tmin_root, tmax_root);
  if(not inter_root || dmax <= tmin_root)
    return false;

  stack.clear();
  stack.push({tree.root, tmin_root, std::min(tmax_root, dmax)});

  while(not stack.empty() && not out.containsOpaqueFace) {
    auto e = stack.pop();
    
    while(not e.node->is_leaf) {
      auto branch = static_cast<Branch*>(e.node);
      real rel = branch->cutValue - ray_orig[branch->cutAxis];
      real tsplit = rel * ray_inv_dir[branch->cutAxis];
      Node* first;
      Node* second;
      if(rel >= 0) {
        first = branch->left;
        second = branch->right;
      }
      else {
        first = branch->right;
        second = branch->left;
      }
      if(tsplit >= e.tmax || tsplit < 0)
        e.node = first;
      else if (tsplit <= e.tmin)
        e.node = second;
      else {
        stack.push({second, tsplit, e.tmax});
        e.node = first;
        e.tmax = tsplit;
      }
    }

    auto leaf = static_cast<Leaf*>(e.node);
    intersectSetOfTrianglesLighting(ray_orig, ray_dir, leaf->facePtrs,
      leaf->facesCount, vertices, materials, faceToIgnore, dmax, out);
  }
  if(not out.containsOpaqueFace)
    out.finalize();
  return out.containsOpaqueFace || not out.empty();
}

} // namespace proxo
