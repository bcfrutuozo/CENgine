#pragma once

#include "Graphics.h"
#include "Job.h"

#include <vector>

class RenderQueue
{
public:

	void Accept(Job job) noexcept
	{
		jobs.push_back(job);
	}

	void Execute(Graphics& graphics) const NOXND
	{
		for(const auto& j : jobs)
		{
			j.Execute(graphics);
		}
	}

	void Reset() noexcept
	{
		jobs.clear();
	}
private:

	std::vector<Job> jobs;
};

