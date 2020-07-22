#include "ShadowRasterizer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	ShadowRasterizer::ShadowRasterizer(Graphics& graphics, int depthBias, float slopeBias, float clamp)
	{
		ChangeDepthBiasParameters(graphics, depthBias, slopeBias, clamp);
	}

	void ShadowRasterizer::ChangeDepthBiasParameters(Graphics& graphics, int depthBias_in, float slopeBias_in, float clamp_in)
	{
		depthBias = depthBias_in;
		slopeBias = slopeBias_in;
		clamp = clamp_in;

		INFOMAN(graphics);

		D3D11_RASTERIZER_DESC rDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});

		rDesc.DepthBias = depthBias_in;
		rDesc.SlopeScaledDepthBias = slopeBias_in;
		rDesc.DepthBiasClamp = clamp_in;

		GFX_THROW_INFO(GetDevice(graphics)->CreateRasterizerState(&rDesc, &pRasterizer));
	}

	int ShadowRasterizer::GetDepthBias() const
	{
		return depthBias;
	}

	float ShadowRasterizer::GetSlopeBias() const
	{
		return slopeBias;
	}

	float ShadowRasterizer::GetClamp() const
	{
		return clamp;
	}

	void ShadowRasterizer::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->RSSetState(pRasterizer.Get()));
	}
}
