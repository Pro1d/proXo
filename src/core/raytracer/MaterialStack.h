#ifndef __MATERIAL_STACK_H__
#define __MATERIAL_STACK_H__

#include "core/math/type.h"

namespace proxo {

struct InnerMaterial {
	real refractive_index;
	real absorption[VEC3_SCALARS_COUNT];
};

class MaterialStack {
public:
	MaterialStack(positive maxSize, real refractive_index, real absorption);
	MaterialStack(positive maxSize, real refractive_index, vec3 absorption);
	~MaterialStack();

	void push(const InnerMaterial& mat);
	void push(real refractive_index, vec3 absorption);
	void push(real refractive_index, real absorption);
	void push(real refractive_index, real absorption, vec3 color);
	const InnerMaterial& top();
	void pop();
	bool empty();
	const InnerMaterial* topPtr();
	void pop(const InnerMaterial*& topPtr);
private:
	InnerMaterial*const stack;
	InnerMaterial* stackTop;
};

inline void MaterialStack::push(const InnerMaterial& mat)
{
	++stackTop;
	*stackTop = mat;
}

inline void MaterialStack::push(real refractive_index, vec3 absorption)
{
	++stackTop;
	stackTop->refractive_index = refractive_index;
	stackTop->absorption[0]    = absorption[0];
	stackTop->absorption[1]    = absorption[1];
	stackTop->absorption[2]    = absorption[2];
}

inline void MaterialStack::push(real refractive_index, real absorption)
{
	++stackTop;
	stackTop->refractive_index = refractive_index;
	stackTop->absorption[0]    = absorption;
	stackTop->absorption[1]    = absorption;
	stackTop->absorption[2]    = absorption;
}

inline void MaterialStack::push(
    real refractive_index, real absorption, vec3 color)
{
	++stackTop;
	stackTop->refractive_index = refractive_index;
	stackTop->absorption[0]    = absorption * (1 - color[0]);
	stackTop->absorption[1]    = absorption * (1 - color[1]);
	stackTop->absorption[2]    = absorption * (1 - color[2]);
}

inline const InnerMaterial& MaterialStack::top()
{
	return *stackTop;
}

inline void MaterialStack::pop()
{
	if(!empty())
		--stackTop;
}

inline bool MaterialStack::empty()
{
	return stackTop == stack;
}

inline const InnerMaterial* MaterialStack::topPtr()
{
	return stackTop;
}

inline void MaterialStack::pop(const InnerMaterial*& topPtr)
{
	if(topPtr != stack)
		--topPtr;
}

} // namespace proxo

#endif // __MATERIAL_STACK_H__
