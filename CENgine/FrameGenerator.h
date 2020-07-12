#pragma once

#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "RenderQueue.h"
#include "Log.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Blur.h"

#include <array>

class FrameGenerator
{
public:

	FrameGenerator(Graphics& graphics)
		:
		depthStencil(graphics, graphics.GetWidth(), graphics.GetHeight()),
		renderTarget1({ graphics, graphics.GetWidth() / downFactor, graphics.GetHeight() / downFactor }),
		renderTarget2({ graphics, graphics.GetWidth() / downFactor, graphics.GetHeight() / downFactor }),
		blur(graphics, 7, 2.6f, "BlurOutline_PS.cso")
	{
		// Setup fullscreen geometry
		CENgineexp::VertexLayout layout;
		layout.Append(CENgineexp::VertexLayout::Position2D);
		CENgineexp::VertexBuffer vBuf{ layout };
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ 1, 1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ -1, -1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ 1, -1 });
		pVertexBuffer = Bind::VertexBuffer::Resolve(graphics, "$Full", std::move(vBuf));
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		pIndexBuffer = Bind::IndexBuffer::Resolve(graphics, "$Full", std::move(indices));

		// Setup fullscreen shaders
		pVertexShader = Bind::VertexShader::Resolve(graphics, "Fullscreen_VS.cso");
		pInputLayout = Bind::InputLayout::Resolve(graphics, layout, pVertexShader->GetByteCode());
		pSamplerFullPoint = Bind::Sampler::Resolve(graphics, false, true);
		pSamplerFullBilin = Bind::Sampler::Resolve(graphics, true, true);
		pBlenderMerge = Bind::Blender::Resolve(graphics, true);
	}

	void Accept(Job job, size_t target) noexcept
	{
		queue[target].Accept(job);
	}

	void Execute(Graphics& graphics) NOXND
	{
		// Normally this would be a loop with each pass defining it setup / etc.
		// and later on it would be a complex graph with parallel execution contingent
		// on input / output requirements

		// Setup render target used for normal passes
		depthStencil.Clear(graphics);
		renderTarget1->Clear(graphics);
		graphics.BindSwapBuffer(depthStencil);

		// Main phong lighting pass
		Bind::Blender::Resolve(graphics, false)->Bind(graphics);
		Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Off)->Bind(graphics);
		queue[0].Execute(graphics);

		// Outline masking pass
		Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Write)->Bind(graphics);
		Bind::NullPixelShader::Resolve(graphics)->Bind(graphics);
		queue[1].Execute(graphics);
		// Outline drawing pass
		renderTarget1->BindAsTarget(graphics);
		Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Off)->Bind(graphics);
		queue[2].Execute(graphics);

		// Fullscreen blur h-passe
		renderTarget2->BindAsTarget(graphics);
		renderTarget1->BindAsTexture(graphics, 0);
		pVertexBuffer->Bind(graphics);
		pIndexBuffer->Bind(graphics);
		pVertexShader->Bind(graphics);
		pInputLayout->Bind(graphics);
		pSamplerFullPoint->Bind(graphics);
		blur.Bind(graphics);
		blur.SetHorizontal(graphics);
		graphics.DrawIndexed(pIndexBuffer->GetCount());

		// fullscreen blur v-pass + combine
		graphics.BindSwapBuffer(depthStencil);
		renderTarget2->BindAsTexture(graphics, 0u);
		pBlenderMerge->Bind(graphics);
		pSamplerFullBilin->Bind(graphics);
		Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Mask)->Bind(graphics);
		blur.SetVertical(graphics);
		graphics.DrawIndexed(pIndexBuffer->GetCount());
	}

	void Reset() noexcept
	{
		for(auto& p : queue)
		{
			p.Reset();
		}
	}

	void ShowWindows(Graphics& graphics)
	{
		if(ImGui::Begin("Blur"))
		{
			if(ImGui::SliderInt("Down Factor", &downFactor, 1, 16))
			{
				renderTarget1.emplace(graphics, graphics.GetWidth() / downFactor, graphics.GetHeight() / downFactor);
				renderTarget2.emplace(graphics, graphics.GetWidth() / downFactor, graphics.GetHeight() / downFactor);
			}
			blur.RenderWidgets(graphics);
		}
		ImGui::End();
	}

private:

	std::array<RenderQueue, 3> queue;
	int downFactor = 1;
	DepthStencil depthStencil;
	std::optional<RenderTarget> renderTarget1;
	std::optional<RenderTarget> renderTarget2;
	Blur blur;
	std::shared_ptr<Bind::VertexBuffer> pVertexBuffer;
	std::shared_ptr<Bind::IndexBuffer> pIndexBuffer;
	std::shared_ptr<Bind::VertexShader> pVertexShader;
	std::shared_ptr<Bind::InputLayout> pInputLayout;
	std::shared_ptr<Bind::Sampler> pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> pSamplerFullBilin;
	std::shared_ptr<Bind::Blender> pBlenderMerge;
};

