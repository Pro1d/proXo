#include "Intersection.h"
#include "../math/Vector.h"
#include "../math/type.h"
#include <limits>
#include <algorithm>

#define EPSILON         ((real) 0.00001)

integer intersectTriangle(vec3 orig, vec3 dir, vec3 vert0, vec3 vert1, vec3 vert2, real * t, real * u, real * v) {
    real edge1[VEC4_SCALARS_COUNT], edge2[VEC4_SCALARS_COUNT];
    real tvec[VEC4_SCALARS_COUNT], pvec[VEC4_SCALARS_COUNT], qvec[VEC4_SCALARS_COUNT];
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
    if(*v < 0 || *u+*v > det)
        return 0;

    *t = dot(edge2, qvec);
    inv_det = 1 / det;
    *t *= inv_det;
    *u *= inv_det;
    *v *= inv_det;

    return side;
}

// paralSize = width / 2, height / 2, depth / 2
real intersectionRayAxialParallelepiped(vec4 orig, vec4 dir, vec3 paralMin, vec3 paralMax) {
    // ray is of form R + t D; assign min t as thit; normal N
    real tmin = 0, tmax = std::numeric_limits<real>::infinity();
    // intersect ray with x, y, z ``slabs'' (k = 0, 1, 2)
    for(int k = 0; k < 3; k++) {
        if(dir[k] != 0) {
            real t1 = (paralMin[k] - orig[k]) / dir[k]; // plane x_k = -dx_k
            real t2 = (paralMax[k] - orig[k]) / dir[k]; // plane x_k = +dx_k
            tmin = std::max(tmin, std::min (t1, t2)); // intersect [tmin..
            tmax = std::min(tmax, std::max (t1, t2)); // tmax], [t1..t2]
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
