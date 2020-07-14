#pragma once

#include "Conditional_noexcept.h"

class Drawable;
class Graphics;
class Step;

namespace RGP
{
	class Job
	{
	public:

		Job(const Step* pStep, const Drawable* pDrawable);
		void Execute(Graphics& graphics) const NOXND;
	private:

		const class Drawable* pDrawable;
		const class Step* pStep;
	};
}