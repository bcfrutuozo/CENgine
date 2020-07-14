#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"

#include <vector>

class Graphics;

namespace RGP
{
	class LambertianPass : public RenderQueuePass
	{
	public:

		LambertianPass(Graphics& graphics, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Off));
		}

		void BindMainCamera(const Camera& camera) noexcept
		{
			pMainCamera = &camera;
		}

		void Execute(Graphics& graphics) const NOXND override
		{
			assert(pMainCamera);

			pMainCamera->BindToGraphics(graphics);
			RenderQueuePass::Execute(graphics);
		}

	private:

		const Camera* pMainCamera = nullptr;
	};
}