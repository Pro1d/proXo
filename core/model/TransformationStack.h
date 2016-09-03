#include "../math/type.h"
#include "TransformationStack.h"


class TransformationStack {
	public:
		TransformationStack(positive stack_size);
		~TransformationStack();
		extern inline void save();
		extern inline void restore();
		extern inline mat4 getMatrix();
		extern inline void preMult(mat4 M);
		extern inline void postMult(mat4 M);
	private:
		mat4 stack;
}
	