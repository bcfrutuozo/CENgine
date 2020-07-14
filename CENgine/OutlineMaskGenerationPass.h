#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "NullPixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

#include <vector>

class Graphics;

namespace RGP
{
	class OutlineMaskGenerationPass : public RenderQueuePass
	{
	public:

		OutlineMaskGenerationPass(Graphics& graphics, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::VertexShader::Resolve(graphics, "Solid_VS.cso"));
			AddBind(Bind::NullPixelShader::Resolve(graphics));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Write));
			AddBind(Bind::Rasterizer::Resolve(graphics, false));
		}
	};
}