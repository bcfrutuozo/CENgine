#pragma once

#include "RenderGraph.h"
#include "DynamicConstantBuffer.h"

#include <memory>

class Graphics;
class Camera;

namespace Bind
{
	class Bindable;
	class RenderTarget;
}

namespace RGP
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:

		BlurOutlineRenderGraph(Graphics& graphics);
		void RenderWidgets(Graphics& graphics);
		void BindMainCamera(Camera& camera);
		void BindShadowCamera(Camera& camera);
	private:

		enum class KernelType
		{
			Gauss,
			Box,
		} kernelType = KernelType::Gauss;

		void SetKernelGauss(int radius, float sigma) NOXND;
		void SetKernelBox(int radius) NOXND;

		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::DynamicCachingPixelConstantBuffer> blurKernel;
		std::shared_ptr<Bind::DynamicCachingPixelConstantBuffer> blurDirection;
	};
}
