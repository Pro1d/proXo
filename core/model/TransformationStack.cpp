#include <cstring>
#include "../math/type.h"
#include "../math/Matrix.h"
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

