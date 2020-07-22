#include "ShadowSampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	ShadowSampler::ShadowSampler(Graphics& graphics)
	{
		for(size_t i = 0; i < 4; i++)
		{
			currentSampler = i;
			samplers[i] = MakeSampler(graphics, IsBilinear(), HasHardwarePcf());
		}

		// Pre-bind something to both sampler slots by default so that no slot is unbound during drawing
		SetBilinear(true);
		SetHardwarePcf(true);
	}

	void ShadowSampler::SetBilinear(bool isBilinear)
	{
		currentSampler = (currentSampler & ~0b01) | (isBilinear ? 0b01 : 0);
	}

	void ShadowSampler::SetHardwarePcf(bool hasHardwarePcf)
	{
		currentSampler = (currentSampler & ~0b10) | (hasHardwarePcf ? 0b10 : 0);
	}

	bool ShadowSampler::IsBilinear() const
	{
		return currentSampler & 0b01;
	}

	bool ShadowSampler::HasHardwarePcf() const
	{
		return currentSampler & 0b10;
	}

	UINT ShadowSampler::GetCurrentSlot() const
	{
		return HasHardwarePcf() ? 1 : 2;
	}

	size_t ShadowSampler::ShadowSamplerIndex(bool isBilinear, bool hasHardwarePcf)
	{
		return (isBilinear ? 0b01 : 0) + (hasHardwarePcf ? 0b10 : 0);
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowSampler::MakeSampler(Graphics& graphics, bool isBilinear, bool hasHardwarePcf)
	{
		INFOMAN(graphics);

		D3D11_SAMPLER_DESC sDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT {} };

		sDesc.BorderColor[0] = 1.0f;
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

		if(hasHardwarePcf)
		{
			sDesc.Filter = isBilinear ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			sDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		else
		{
			sDesc.Filter = isBilinear ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
		}

		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		GFX_THROW_INFO(GetDevice(graphics)->CreateSamplerState(&sDesc, &pSampler));
		return std::move(pSampler);
	}

	void ShadowSampler::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->PSSetSamplers(GetCurrentSlot(), 1, samplers[currentSampler].GetAddressOf()));
	}
}