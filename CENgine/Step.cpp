#include "Step.h"
#include "Drawable.h"
#include "FrameGenerator.h"

void Step::Submit(class FrameGenerator& frame, const class Drawable& drawable) const
{
	frame.Accept(Job { this, &drawable }, targetQueue);
}

void Step::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for(auto& b : bindables)
	{
		b->InitializeParentReference(parent);
	}
}