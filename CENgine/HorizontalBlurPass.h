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
	class HorizontalBlurPass : public FullscreenPass
	{
	public:

		HorizontalBlurPass(std::string name, Graphics& graphics, unsigned int fullWidth, unsigned int fullHeight);
		void Execute(Graphics& graphics) const NOXND override;
	private:

		std::shared_ptr<Bind::DynamicCachingPixelConstantBuffer> direction;
	};
}