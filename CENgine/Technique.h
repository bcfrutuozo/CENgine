#pragma once

#include "Step.h"
#include "TechniqueProbe.h"

#include <vector>

class Technique
{ 
public:

	Technique() = default;
	Technique(std::string name, bool startActive = true) noexcept
		:
		name(name),
		isActive(startActive)
	{ }

	void Submit(class FrameGenerator& frame, const class Drawable& drawable) const noexcept;

	void AddStep(Step step) noexcept
	{
		steps.push_back(std::move(step));
	}

	bool IsActive() const noexcept
	{
		return isActive;
	}

	void SetActiveState(bool isActive_in) noexcept
	{
		isActive = isActive_in;
	}

	void InitializeParentReferences(const class Drawable& parent) noexcept;

	void Accept(TechniqueProbe& probe)
	{
		probe.SetTechnique(this);
		for(auto& s : steps)
		{
			s.Accept(probe);
		}
	}

	const std::string& GetName() const noexcept
	{
		return name;
	}
private:

	bool isActive = true;
	std::vector<Step> steps;
	std::string name = "Nameless Technique";
};

