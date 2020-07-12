#include "Job.h"
#include "Step.h"
#include "Drawable.h"


Job::Job(const class Step* pStep, const class Drawable* pDrawable)
	:
	pDrawable{ pDrawable },
	pStep{ pStep }
{}

void Job::Execute(class Graphics& graphics) const NOXND
{
	pDrawable->Bind(graphics);
	pStep->Bind(graphics);
	graphics.DrawIndexed(pDrawable->GetIndexCount());
}