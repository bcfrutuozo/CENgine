#include "RenderQueuePass.h"

namespace RGP
{
	void RenderQueuePass::Accept(Job job) noexcept
	{
		jobs.push_back(job);
	}

	void RenderQueuePass::Execute(Graphics& graphics) const NOXND
	{
		BindAll(graphics);

		for(const auto& j : jobs)
		{
			j.Execute(graphics);
		}
	}

	void RenderQueuePass::Reset() NOXND
	{
		jobs.clear();
	}
}