#include "ShadowSampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	ShadowSampler::ShadowSampler(Graphics& graphics)
	{
		INFOMAN(graphics);

		D3D11_SAMPLER_DESC sDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT {} };

		sDesc.BorderColor[0] = 1.0f;
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

		GFX_THROW_INFO(GetDevice(graphics)->CreateSamplerState(&sDesc, &pSampler));
	}

	void ShadowSampler::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->PSSetSamplers(1, 1, pSampler.GetAddressOf()));
	}
}