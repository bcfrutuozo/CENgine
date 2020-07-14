#pragma once

#include "BindingPass.h"
#include "Job.h"

#include <vector>

namespace RGP
{
	class RenderQueuePass : public BindingPass
	{
	public:

		using BindingPass::BindingPass;

		void Accept(Job job) noexcept;
		void Execute(Graphics& graphics) const NOXND override;
		void Reset() NOXND override;
	private:

		std::vector<Job> jobs;
	};
}
