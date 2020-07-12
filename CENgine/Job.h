#pragma once

#include "Conditional_noexcept.h"

class Job
{
public:

	Job(const class Step* pStep, const class Drawable* pDrawable);
	void Execute(class Graphics& graphics) const NOXND;
private:

	const class Drawable* pDrawable;
	const class Step* pStep;
};

