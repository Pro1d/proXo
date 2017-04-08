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
	MaterialStack(const MaterialStack & m);
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

private:
	InnerMaterial*const stack_;
	InnerMaterial* stackTop_;
	positive maxSize_;
};

inline void MaterialStack::push(const InnerMaterial& mat)
{
	++stackTop_;
	*stackTop_ = mat;
}

inline void MaterialStack::push(real refractive_index, vec3 absorption)
{
	++stackTop_;
	stackTop_->refractive_index = refractive_index;
	stackTop_->absorption[0]    = absorption[0];
	stackTop_->absorption[1]    = absorption[1];
	stackTop_->absorption[2]    = absorption[2];
}

inline void MaterialStack::push(real refractive_index, real absorption)
{
	++stackTop_;
	stackTop_->refractive_index = refractive_index;
	stackTop_->absorption[0]    = absorption;
	stackTop_->absorption[1]    = absorption;
	stackTop_->absorption[2]    = absorption;
}

inline void MaterialStack::push(
    real refractive_index, real absorption, vec3 color)
{
	++stackTop_;
	stackTop_->refractive_index = refractive_index;
	stackTop_->absorption[0]    = absorption * (1 - color[0]);
	stackTop_->absorption[1]    = absorption * (1 - color[1]);
	stackTop_->absorption[2]    = absorption * (1 - color[2]);
}

inline const InnerMaterial& MaterialStack::top()
{
	return *stackTop_;
}

inline void MaterialStack::pop()
{
	if(!empty())
		--stackTop_;
}

inline bool MaterialStack::empty()
{
	return stackTop_ == stack_;
}

} // namespace proxo

#endif // __MATERIAL_STACK_H__
