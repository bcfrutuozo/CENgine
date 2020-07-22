#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"
#include "DepthStencil.h"
#include "ShadowCameraCbuf.h"
#include "ShadowSampler.h"

#include <vector>

class Graphics;

namespace RGP
{
	class LambertianPass : public RenderQueuePass
	{
	public:

		LambertianPass(Graphics& graphics, std::string name)
			:
			RenderQueuePass(std::move(name)),
			pShadowCameraConstantBuffer{ std::make_shared<Bind::ShadowCameraCbuf>(graphics) }
		{
			AddBind(pShadowCameraConstantBuffer);
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBindSink<Bind::Bindable>("shadowMap");
			AddBind(std::make_shared<Bind::ShadowSampler>(graphics));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Off));
		}

		void BindMainCamera(const Camera& camera) noexcept
		{
			pMainCamera = &camera;
		}

		void BindShadowCamera(const Camera& camera) noexcept
		{
			pShadowCameraConstantBuffer->SetCamera(&camera);
		}

		void Execute(Graphics& graphics) const NOXND override
		{
			assert(pMainCamera);
			pShadowCameraConstantBuffer->Update(graphics);
			pMainCamera->BindToGraphics(graphics);
			RenderQueuePass::Execute(graphics);
		}

	private:

		std::shared_ptr<Bind::ShadowCameraCbuf> pShadowCameraConstantBuffer;
		std::shared_ptr<Bind::Bindable> pShadowMap;
		const Camera* pMainCamera = nullptr;
	};
}