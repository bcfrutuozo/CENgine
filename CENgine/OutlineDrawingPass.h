#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

#include <vector>

class Graphics;

namespace RGP
{
	class OutlineDrawingPass : public RenderQueuePass
	{
	public:

		OutlineDrawingPass(Graphics& graphics, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::VertexShader::Resolve(graphics, "Solid_VS.cso"));
			AddBind(Bind::PixelShader::Resolve(graphics, "Solid_PS.cso"));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Mask));
			AddBind(Bind::Rasterizer::Resolve(graphics, false));
		}
	};
}