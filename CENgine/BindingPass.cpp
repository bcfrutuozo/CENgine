#include "BindingPass.h"
#include "Bindable.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderGraphCompileException.h"

namespace RGP
{
	BindingPass::BindingPass(std::string name, std::vector<std::shared_ptr<Bind::Bindable>> binds)
		:
		Pass(std::move(name)),
		binds(std::move(binds))
	{ }

	void BindingPass::AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept
	{
		binds.push_back(std::move(bind));
	}

	void BindingPass::BindAll(Graphics& graphics) const NOXND
	{
		BindBufferResources(graphics);

		for(auto& bind : binds)
		{
			bind->Bind(graphics);
		}
	}

	void BindingPass::Finalize()
	{
		Pass::Finalize();
		if(!renderTarget && !depthStencil)
		{
			throw RGC_EXCEPTION("Binding Pass [" + GetName() + "] needs at least one of RenderTarget or DepthStencil");
		}
	}

	void BindingPass::BindBufferResources(Graphics& graphics) const NOXND
	{
		if(renderTarget)
		{
			renderTarget->BindAsBuffer(graphics, depthStencil.get());
		}
		else
		{
			depthStencil->BindAsBuffer(graphics);
		}
	}
}