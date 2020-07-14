#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"

#include <vector>

class Graphics;

namespace RGP
{
	class WireframePass : public RenderQueuePass
	{
	public:

		WireframePass(Graphics& graphics, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::DepthReversed));
		}
	};
}