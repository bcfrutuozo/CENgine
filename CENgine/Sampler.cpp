#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {

	Sampler::Sampler(Graphics& graphics, bool isAnisoEnabled, bool isReflect)
		:
		isAnisoEnabled(isAnisoEnabled),
		isReflect(isReflect)
	{
		INFOMAN(graphics);

		D3D11_SAMPLER_DESC sDesc = {};
		sDesc.Filter = isAnisoEnabled ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		sDesc.AddressU = isReflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressV = isReflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

		GFX_THROW_INFO(GetDevice(graphics)->CreateSamplerState(&sDesc, &pSampler));
	}

	void Sampler::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& graphics, bool isAnisoEnabled, bool isReflect)
	{
		return Codex::Resolve<Sampler>(graphics, isAnisoEnabled, isReflect);
	}

	std::string Sampler::GenerateUID(bool isAnisoEnabled, bool isReflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + (isAnisoEnabled ? "A"s : "a"s) + (isReflect ? "R"s : "W"s);
	}

	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(isAnisoEnabled, isReflect);
	}
}