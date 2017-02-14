#include <cstring>
#include "core/math/type.h"
#include "core/math/Matrix.h"
#include "TransformationStack.h"

TransformationStack::TransformationStack(positive stack_size) :
		stack(new real[stack_size*MAT4_SCALARS_COUNT]),
		stack_top(stack)
{
	identity(stack_top);
}

TransformationStack::~TransformationStack() {
	delete[] stack;
}

integer TransformationStack::height() {
    return (stack_top - stack) / MAT4_SCALARS_COUNT;
}

