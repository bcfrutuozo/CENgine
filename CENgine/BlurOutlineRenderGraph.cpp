#include "BlurOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"
#include "Source.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "BlurOutlineDrawingPass.h"
#include "WireframePass.h"
#include "ShadowMappingPass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "Math.h"
#include "imgui/imgui.h"
#include "SkyboxPass.h"

namespace RGP
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& graphics)
		:
		RenderGraph(graphics)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<ShadowMappingPass>(graphics, "shadowMap");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<LambertianPass>(graphics, "lambertian");
			pass->SetSinkLinkage("shadowMap", "shadowMap.map");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<SkyboxPass>(graphics, "skybox");
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(graphics, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "skybox.depthStencil");
			AppendPass(std::move(pass));
		}

		// setup blur constant buffers
		{
			{
				DRR::IncompleteLayout l;
				l.Add<DRR::Type::Integer>("nTaps");
				l.Add<DRR::Type::Array>("coefficients");
				l["coefficients"].Set<DRR::Type::Float>(maxRadius * 2 + 1);
				DRR::Buffer buffer{ std::move(l) };
				blurKernel = std::make_shared<Bind::DynamicCachingPixelConstantBuffer>(graphics, buffer, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectBindableSource<Bind::DynamicCachingPixelConstantBuffer>::Make("blurKernel", blurKernel));
			}
			{
				DRR::IncompleteLayout l;
				l.Add<DRR::Type::Bool>("isHorizontal");
				DRR::Buffer buffer{ std::move(l) };
				blurDirection = std::make_shared<Bind::DynamicCachingPixelConstantBuffer>(graphics, buffer, 1);
				AddGlobalSource(DirectBindableSource<Bind::DynamicCachingPixelConstantBuffer>::Make("blurDirection", blurDirection));
			}
		}

		{
			auto pass = std::make_unique<BlurOutlineDrawingPass>(graphics, "outlineDraw", graphics.GetWidth(), graphics.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", graphics, graphics.GetWidth(), graphics.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", graphics);
			pass->SetSinkLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<WireframePass>(graphics, "wireframe");
			pass->SetSinkLinkage("renderTarget", "vertical.renderTarget");
			pass->SetSinkLinkage("depthStencil", "vertical.depthStencil");
			AppendPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "wireframe.renderTarget");

		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) NOXND
	{
		assert(radius <= maxRadius);
		auto k = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;

		for(int i = 0; i < nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = Gauss(x, sigma);
			sum += g;
			k["coefficients"][i] = g;
		}

		for(int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}

		blurKernel->SetBuffer(k);
	}

	void BlurOutlineRenderGraph::SetKernelBox(int radius) NOXND
	{
		assert(radius <= maxRadius);
		auto k = blurKernel->GetBuffer();
		const int nTaps = (radius * 2) + 1;

		k["nTaps"] = nTaps;
		const float c = 1.0f / nTaps;

		for(int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = c;
		}

		blurKernel->SetBuffer(k);
	}

	void BlurOutlineRenderGraph::RenderWidgets(Graphics& graphics)
	{
		RenderShadowWidget(graphics);
		RenderKernelWidget(graphics);
		dynamic_cast<SkyboxPass&>(FindPassByName("skybox")).RenderWidget();
	}

	void BlurOutlineRenderGraph::RenderKernelWidget(Graphics& graphics)
	{
		if(ImGui::Begin("Kernel"))
		{
			bool filterChanged = false;
			{
				const char* items[] = { "Gauss","Box" };
				static const char* curItem = items[0];
				if(ImGui::BeginCombo("Filter Type", curItem))
				{
					for(int n = 0; n < std::size(items); n++)
					{
						const bool isSelected = (curItem == items[n]);
						if(ImGui::Selectable(items[n], isSelected))
						{
							filterChanged = true;
							curItem = items[n];
							if(curItem == items[0])
							{
								kernelType = KernelType::Gauss;
							}
							else if(curItem == items[1])
							{
								kernelType = KernelType::Box;
							}
						}
						if(isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			bool radChange = ImGui::SliderInt("Radius", &radius, 0, maxRadius);
			bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);
			if(radChange || sigChange || filterChanged)
			{
				if(kernelType == KernelType::Gauss)
				{
					SetKernelGauss(radius, sigma);
				}
				else if(kernelType == KernelType::Box)
				{
					SetKernelBox(radius);
				}
			}
		}
		ImGui::End();
	}

	void BlurOutlineRenderGraph::RenderShadowWidget(Graphics& graphics)
	{
		if(ImGui::Begin("Shadow"))
		{
			if(ImGui::Button("Dump Cubemap"))
			{
				DumpShadowMap(graphics, "Dumps\\shadow_");
			}
		}
		ImGui::End();
	}

	void BlurOutlineRenderGraph::BindMainCamera(Camera& camera)
	{
		dynamic_cast<LambertianPass&>(FindPassByName("lambertian")).BindMainCamera(camera);
		dynamic_cast<SkyboxPass&>(FindPassByName("skybox")).BindMainCamera(camera);
	}

	void BlurOutlineRenderGraph::BindShadowCamera(Camera& camera)
	{
		dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMap")).BindShadowCamera(camera);
		dynamic_cast<LambertianPass&>(FindPassByName("lambertian")).BindShadowCamera(camera);
	}

	void BlurOutlineRenderGraph::DumpShadowMap(Graphics& graphics, const std::string& path)
	{
		dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMap")).DumpShadowMap(graphics, path);
	}
}