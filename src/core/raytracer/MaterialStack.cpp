#include "MaterialStack.h"

namespace proxo {

MaterialStack::MaterialStack(positive maxSize)
    : stack(new InnerMaterial[maxSize]), stackTop(stack)
{
}

MaterialStack::~MaterialStack()
{
	delete[] stack;
}

} // namespace proxo
