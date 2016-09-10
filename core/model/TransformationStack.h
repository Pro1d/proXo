#ifndef __TRANSFORMATION_STACK_H__
#define __TRANSFORMATION_STACK_H__

#include "../math/type.h"

class TransformationStack {
	public:
		TransformationStack(positive stack_size);
		~TransformationStack();
		void save();
		void restore();
		mat4 getMatrix();
		void preMult(mat4 M);
		void postMult(mat4 M);
        void saveAndPreMult(mat4 M);
        void pushMult(mat4 M, mat4 N);
	private:
		mat4 stack;
		mat4 stack_top;
		real tmpMat[MAT4_SCALARS_COUNT];
};

#endif
