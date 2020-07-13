#pragma once

#include "Bindable.h"
#include "Graphics.h"

#include <vector>
#include <memory>

class TechniqueProbe;
class Drawable;
class RenderQueuePass;
class RenderGraph;

class Step
{
public:

	Step(std::string targetPass);
	Step(Step&&) = default;
	Step(const Step& source) noexcept;
	Step operator=(const Step&) = delete;
	Step& operator=(const Step&&) = delete;
	void AddBindable(std::shared_ptr<Bind::Bindable> bindables_in) noexcept;
	void Submit(const Drawable& drawable) const;
	void Bind(Graphics& graphics) const;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	void Link(RenderGraph& renderGraph);
private:

	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
	RenderQueuePass* pTargetPass;
	std::string targetPassName;
};

