#pragma once

#include "FullscreenPass.h"
#include "DynamicConstantBuffer.h"

class Graphics;

namespace Bind
{
	class PixelShader;
	class RenderTarget;
}

namespace RGP
{
	class VerticalBlurPass : public FullscreenPass
	{
	public:

		VerticalBlurPass(std::string name, Graphics& graphics);
		void Execute(Graphics& graphics) const NOXND override;
	private:

		std::shared_ptr<Bind::DynamicCachingPixelConstantBuffer> direction;
	};
}