#include "MaterialStack.h"
#include <cstring>

namespace proxo {

MaterialStack::MaterialStack(
    positive maxSize, real refractive_index, real absorption)
    : stack_(new InnerMaterial[maxSize]), stackTop_(stack_), maxSize_(maxSize)
{
	stack_->refractive_index = refractive_index;
	stack_->absorption[0]    = absorption;
	stack_->absorption[1]    = absorption;
	stack_->absorption[2]    = absorption;
}

MaterialStack::MaterialStack(
    positive maxSize, real refractive_index, vec3 absorption)
    : stack_(new InnerMaterial[maxSize]), stackTop_(stack_), maxSize_(maxSize)
{
	stack_->refractive_index = refractive_index;
	stack_->absorption[0]    = absorption[0];
	stack_->absorption[1]    = absorption[1];
	stack_->absorption[2]    = absorption[2];
}

MaterialStack::MaterialStack(const MaterialStack & mat)
    : stack_(new InnerMaterial[mat.maxSize_]),
      stackTop_(stack_+(mat.stackTop_-mat.stack_)),
      maxSize_(mat.maxSize_)
{
	memcpy(stack_, mat.stack_,
	    sizeof(InnerMaterial) * (mat.stackTop_ - mat.stack_ + 1));
}

MaterialStack::~MaterialStack()
{
	delete[] stack_;
}

} // namespace proxo
