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
	MaterialStack(positive maxSize);
	~MaterialStack();

	void push(real refractive_index, vec3 absorption);
	void push(real refractive_index, real absorption, vec3 color);
	const InnerMaterial& top();
	const InnerMaterial& pop();
	bool empty();
	const InnerMaterial* topPtr();
	const InnerMaterial& pop(const InnerMaterial*& topPtr);
private:
	InnerMaterial* stack;
	InnerMaterial* stackTop;
};

inline void MaterialStack::push(real refractive_index, vec3 absorption)
{
	stackTop->refractive_index = refractive_index;
	stackTop->absorption[0]    = absorption[0];
	stackTop->absorption[1]    = absorption[1];
	stackTop->absorption[2]    = absorption[2];
	++stackTop;
}

inline void MaterialStack::push(
    real refractive_index, real absorption, vec3 color)
{
	stackTop->refractive_index = refractive_index;
	stackTop->absorption[0]    = absorption * (1 - color[0]);
	stackTop->absorption[1]    = absorption * (1 - color[1]);
	stackTop->absorption[2]    = absorption * (1 - color[2]);
	++stackTop;
}

inline const InnerMaterial& MaterialStack::top()
{
	if(empty())
		return *stackTop;
	return *(stackTop - 1);
}

inline const InnerMaterial& MaterialStack::pop()
{
	if(empty())
		return *stackTop;
	return *(--stackTop);
}

inline bool MaterialStack::empty()
{
	return stackTop == stack;
}

inline const InnerMaterial* MaterialStack::topPtr()
{
	return stackTop;
}

inline const InnerMaterial& MaterialStack::pop(const InnerMaterial*& topPtr)
{
	if(topPtr != stack)
		return *(--topPtr);
	return *topPtr;
}

} // namespace proxo

#endif // __MATERIAL_STACK_H__
