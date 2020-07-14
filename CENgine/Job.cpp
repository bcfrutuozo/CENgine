#include "Job.h"
#include "Step.h"
#include "Drawable.h"

namespace RGP
{
	Job::Job(const Step* pStep, const Drawable* pDrawable)
		:
		pDrawable{ pDrawable },
		pStep{ pStep }
	{}

	void Job::Execute(Graphics& graphics) const NOXND
	{
		pDrawable->Bind(graphics);
		pStep->Bind(graphics);
		graphics.DrawIndexed(pDrawable->GetIndexCount());
	}
}