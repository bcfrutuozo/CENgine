#include "Technique.h"
#include "Drawable.h"
#include "TechniqueProbe.h"

void Technique::Submit(const Drawable& drawable) const noexcept
{
	if(isActive)
	{
		for(const auto& step : steps)
		{
			step.Submit(drawable);
		}
	}
}

void Technique::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for(auto& s : steps)
	{
		s.InitializeParentReferences(parent);
	}
}

Technique::Technique(std::string name, bool startActive) noexcept
	:
	name(name),
	isActive(startActive)
{ }

void Technique::AddStep(Step step) noexcept
{
	steps.push_back(std::move(step));
}

bool Technique::IsActive() const noexcept
{
	return isActive;
}

void Technique::SetActiveState(bool isActive_in) noexcept
{
	isActive = isActive_in;
}

void Technique::Accept(TechniqueProbe& probe)
{
	probe.SetTechnique(this);
	for(auto& s : steps)
	{
		s.Accept(probe);
	}
}

const std::string& Technique::GetName() const noexcept
{
	return name;
}

void Technique::Link(RenderGraph& renderGraph)
{
	for(auto& step : steps)
	{
		step.Link(renderGraph);
	}
}