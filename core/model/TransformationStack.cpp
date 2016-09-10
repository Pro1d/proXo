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

inline void TransformationStack::save() {
	real *new_top = stack_top + MAT4_SCALARS_COUNT;

	memcpy(stack_top, new_top, MAT4_SIZE);

	stack_top = new_top;
}

inline void TransformationStack::restore() {
	stack_top -= MAT4_SCALARS_COUNT;
}

inline mat4 TransformationStack::getMatrix() {
	return stack_top;
}

inline void TransformationStack::preMult(mat4 M) {
	multiplyMM(M, stack_top, tmpMat);
	memcpy(tmpMat, stack_top, MAT4_SIZE);
}

inline void TransformationStack::postMult(mat4 M) {
	multiplyMM(stack_top, M, tmpMat);
	memcpy(tmpMat, stack_top, MAT4_SIZE);
}

inline void TransformationStack::saveAndPreMult(mat4 M) {
	mat4 new_top = stack_top + MAT4_SCALARS_COUNT;
	multiplyMM(M, stack_top, new_top);
	stack_top = new_top;
}

inline void TransformationStack::pushMult(mat4 M, mat4 N) {
	stack_top += MAT4_SCALARS_COUNT;
	multiplyMM(M, N, stack_top);
}
