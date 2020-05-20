#include "Sampler.h"
#include "GraphicsThrowMacros.h"

Sampler::Sampler(Graphics& graphics)
{
	INFOMAN(graphics);

	D3D11_SAMPLER_DESC sDesc = {};
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO(GetDevice(graphics)->CreateSamplerState(&sDesc, &pSampler));	
}

void Sampler::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
}