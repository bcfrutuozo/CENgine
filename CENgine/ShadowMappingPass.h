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
#include "NullPixelShader.h"

#include <vector>

class Graphics;

namespace RGP
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:

		ShadowMappingPass(Graphics& graphics, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			depthStencil = std::make_unique<Bind::ShaderInputDepthStencil>(graphics, 3, Bind::DepthStencil::Usage::ShadowDepth);
			AddBind(Bind::VertexShader::Resolve(graphics, "Solid_VS.cso"));
			AddBind(Bind::NullPixelShader::Resolve(graphics));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Off));
			AddBind(Bind::Blender::Resolve(graphics, false));
			RegisterSource(DirectBindableSource<Bind::DepthStencil>::Make("map", depthStencil));
		}

		void BindShadowCamera(const Camera& camera) noexcept
		{
			pShadowCamera = &camera;
		}

		void Execute(Graphics& graphics) const NOXND override
		{
			depthStencil->Clear(graphics);
			pShadowCamera->BindToGraphics(graphics);
			RenderQueuePass::Execute(graphics);
		}

		void DumpShadowMap(Graphics& graphics, const std::string& path) const
		{
			depthStencil->ToSurface(graphics).Save(path);
		}

	private:

		const Camera* pShadowCamera = nullptr;
	};
}