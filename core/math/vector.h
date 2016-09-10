#ifndef __VECTOR_H__
#define __VECTOR_H__

extern inline real dot(vec4 a, vec4 b);
extern inline real cross(vec4 a, vec4 b);
extern inline void normalize(vec4 in, vec4 out);
extern inline void reflect(vec4 i, vec4 normal, vec4 reflectOut);
extern inline real squaredLength(vec3);
extern inline void substract(vec4 a, vec4 b, vec4 out);
extern inline void multiply(vec4 a, real k);

#endif // __VECTOR_H__
