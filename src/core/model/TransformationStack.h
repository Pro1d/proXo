#ifndef __TRANSFORMATION_STACK_H__
#define __TRANSFORMATION_STACK_H__

#include <cstring>
#include "../math/type.h"
#include "../math/Matrix.h"

class TransformationStack {
	public:
		TransformationStack(positive stack_size);
		~TransformationStack();
		void save();
		void restore();
		void reset();
		mat4 getMatrix();
		void preMult(mat4 M);
		void postMult(mat4 M);
        void saveAndPostMult(mat4 M);
        void pushMult(mat4 M, mat4 N);
        integer height();
		mat4 stack;
	private:
		mat4 stack_top;
		real tmpMat[MAT4_SCALARS_COUNT];
};

inline void TransformationStack::save() {
	real *new_top = stack_top + MAT4_SCALARS_COUNT;

	memcpy(new_top, stack_top, MAT4_SIZE);

	stack_top = new_top;
}

inline void TransformationStack::restore() {
	stack_top -= MAT4_SCALARS_COUNT;
}

inline void TransformationStack::reset() {
    stack_top = stack;
    identity(stack_top);
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
	memcpy(stack_top, tmpMat, MAT4_SIZE);
}

inline void TransformationStack::saveAndPostMult(mat4 M) {
	mat4 new_top = stack_top + MAT4_SCALARS_COUNT;
	multiplyMM(stack_top, M, new_top);
	stack_top = new_top;
}

inline void TransformationStack::pushMult(mat4 M, mat4 N) {
	stack_top += MAT4_SCALARS_COUNT;
	multiplyMM(M, N, stack_top);
}

#endif
