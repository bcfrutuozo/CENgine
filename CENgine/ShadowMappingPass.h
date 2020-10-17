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
#include "ShadowRasterizer.h"
#include "CubeTexture.h"
#include "Viewport.h"
#include "Math.h"

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
			depthStencil = std::make_shared<Bind::OutputOnlyDepthStencil>(graphics, size, size);
			pDepthCube = std::make_shared<Bind::CubeTargetTexture>(graphics, size, size, 3, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);
			AddBind(Bind::VertexShader::Resolve(graphics, "Shadow_VS.cso"));
			AddBind(Bind::PixelShader::Resolve(graphics, "Shadow_PS.cso"));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Off));
			AddBind(Bind::Blender::Resolve(graphics, false));
			AddBind(std::make_shared<Bind::Viewport>(graphics, static_cast<float>(size), static_cast<float>(size)));
			AddBind(std::make_shared<Bind::Rasterizer>(graphics, false));
			RegisterSource(DirectBindableSource<Bind::CubeTargetTexture>::Make("map", pDepthCube));

			DirectX::XMStoreFloat4x4(
				&projection,
				DirectX::XMMatrixPerspectiveFovLH(PI / 2.0f, 1.0f, 0.5f, 100.0f)
			);
			// +x
			DirectX::XMStoreFloat3(&cameraDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// -x
			DirectX::XMStoreFloat3(&cameraDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// +y
			DirectX::XMStoreFloat3(&cameraDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[2], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			// -y
			DirectX::XMStoreFloat3(&cameraDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[3], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			// +z
			DirectX::XMStoreFloat3(&cameraDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// -z
			DirectX::XMStoreFloat3(&cameraDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		}

		void BindShadowCamera(const Camera& camera) noexcept
		{
			pShadowCamera = &camera;
		}

		void Execute(Graphics& graphics) const NOXND override
		{
			using namespace DirectX;
			const auto pos = XMLoadFloat3(&pShadowCamera->GetPosition());

			graphics.SetProjection(XMLoadFloat4x4(&projection));
			for (size_t i = 0; i < 6; i++)
			{
				auto rt = pDepthCube->GetRenderTarget(i);
				rt->Clear(graphics);
				depthStencil->Clear(graphics);
				SetRenderTarget(std::move(rt));
				const auto lookAt = pos + XMLoadFloat3(&cameraDirections[i]);
				graphics.SetCamera(XMMatrixLookAtLH(pos, lookAt, XMLoadFloat3(&cameraUps[i])));
				RenderQueuePass::Execute(graphics);
			}
		}

		void DumpShadowMap(Graphics& graphics, const std::string& path) const
		{
			/*for(size_t i = 0; i < 6; i++)
			{
				auto d = pDepthCube->GetRenderTarget(i);

				d->ToSurface(graphics).Save(path + std::to_string(i) + ".png");
			}*/
		}

	private:

		void SetRenderTarget(std::shared_ptr<Bind::RenderTarget> p_RenderTarget) const
		{
			const_cast<ShadowMappingPass*>(this)->renderTarget = std::move(p_RenderTarget);
		}

		static constexpr UINT size = 1000;
		const Camera* pShadowCamera = nullptr;
		std::shared_ptr<Bind::CubeTargetTexture> pDepthCube;
		DirectX::XMFLOAT4X4 projection;
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	};
}