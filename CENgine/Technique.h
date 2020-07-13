#pragma once

#include "Step.h"

#include <vector>

class TechniqueProbe;
class Drawable;
class RenderGraph;

class Technique
{ 
public:

	Technique() = default;
	Technique(std::string name, bool startActive = true) noexcept;
	void Submit(const Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState(bool active) noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const noexcept;
	void Link(RenderGraph& renderGraph);
private:

	bool isActive = true;
	std::vector<Step> steps;
	std::string name;
};

