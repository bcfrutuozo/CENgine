#include "Step.h"
#include "Drawable.h"
#include "RenderGraph.h"
#include "TechniqueProbe.h"
#include "RenderQueuePass.h"

void Step::Submit(const Drawable& drawable) const
{
	pTargetPass->Accept(RGP::Job{ this, &drawable });
}

void Step::InitializeParentReferences(const class Drawable& parent) noexcept
{
	for(auto& b : bindables)
	{
		b->InitializeParentReference(parent);
	}
}

Step::Step(std::string targetPassName)
	:
	targetPassName{ std::move( targetPassName ) }
{ }

Step::Step(const Step& source) noexcept
	:
	targetPassName(source.targetPassName)
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

void Step::AddBindable(std::shared_ptr<Bind::Bindable> bindables_in) noexcept
{
	bindables.push_back(std::move(bindables_in));
}

void Step::Bind(Graphics& graphics) const NOXND
{
	for(const auto& b : bindables)
	{
		b->Bind(graphics);
	}
}

void Step::Accept(TechniqueProbe& probe)
{
	probe.SetStep(this);
	for(auto& pb : bindables)
	{
		pb->Accept(probe);
	}
}

void Step::Link(RGP::RenderGraph& renderGraph)
{
	assert(pTargetPass == nullptr);
	pTargetPass = &renderGraph.GetRenderQueue(targetPassName);
}