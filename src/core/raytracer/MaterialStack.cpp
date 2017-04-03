#include "MaterialStack.h"

namespace proxo {

MaterialStack::MaterialStack(positive maxSize, real refractive_index, real absorption)
	: stack(new InnerMaterial[maxSize]),
	stackTop(stack)
{
	stack->refractive_index = refractive_index;
	stack->absorption[0] = absorption;
	stack->absorption[1] = absorption;
	stack->absorption[2] = absorption;
}

MaterialStack::MaterialStack(positive maxSize, real refractive_index, vec3 absorption)
    : stack(new InnerMaterial[maxSize]),
	stackTop(stack)
{
	stack->refractive_index = refractive_index;
	stack->absorption[0] = absorption[0];
	stack->absorption[1] = absorption[1];
	stack->absorption[2] = absorption[2];
}

MaterialStack::~MaterialStack()
{
	delete[] stack;
}

} // namespace proxo
