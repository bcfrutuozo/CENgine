#pragma once

#include "Bindable.h"
#include "Graphics.h"
#include "TechniqueProbe.h"

#include <vector>
#include <memory>

class Step
{ 
public:

	Step(size_t targetQueue)
		:
		targetQueue(targetQueue)
	{ }

	Step(Step&&) = default;
	Step(const Step& source) noexcept
		:
		targetQueue(source.targetQueue)
	{
		bindables.reserve(source.bindables.size());
		for(auto& pb : source.bindables)
		{
			if(auto* pCloning = dynamic_cast<const Bind::CloningBindable*>(pb.get()))
			{
				bindables.push_back(pCloning->Clone());
			}
			else
			{
				bindables.push_back(pb);
			}
		}
	}

	Step& operator=(const Step&) = delete;
	Step& operator=(Step &&) = delete;

	void AddBindable(std::shared_ptr<Bind::Bindable> bindables_in) noexcept
	{
		bindables.push_back(std::move(bindables_in));
	}

	void Submit(class FrameGenerator& frame, const class Drawable& drawable) const;

	void Bind(Graphics& graphics) const
	{
		for(const auto& b : bindables)
		{
			b->Bind(graphics);
		}
	}

	void InitializeParentReferences(const class Drawable& parent) noexcept;

	void Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);
		for(auto& pb : bindables)
		{
			pb->Accept(probe);
		}
	}
private:

	size_t targetQueue;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};

