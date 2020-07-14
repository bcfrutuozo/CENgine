#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Blender.h"

#include <vector>

class Graphics;

namespace RGP
{
	class BlurOutlineDrawingPass : public RenderQueuePass
	{
	public:

		BlurOutlineDrawingPass(Graphics& graphics, std::string name, unsigned int fullWidth, unsigned int fullHeight)
			:
			RenderQueuePass(std::move(name))
		{
			renderTarget = std::make_unique<Bind::ShaderInputRenderTarget>(graphics, fullWidth / 2, fullHeight / 2, 0);
			AddBind(Bind::VertexShader::Resolve(graphics, "Solid_VS.cso"));
			AddBind(Bind::PixelShader::Resolve(graphics, "Solid_PS.cso"));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Mask));
			AddBind(Bind::Blender::Resolve(graphics, false));
			RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", renderTarget));
		}

		void Execute(Graphics& graphics) const NOXND override
		{
			renderTarget->Clear(graphics);
			RenderQueuePass::Execute(graphics);
		}
	};
}