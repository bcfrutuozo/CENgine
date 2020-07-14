#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {

	Sampler::Sampler(Graphics& graphics, Type type, bool isReflect)
		:
		type(type),
		isReflect(isReflect)
	{
		INFOMAN(graphics);

		D3D11_SAMPLER_DESC sDesc = {};
		sDesc.Filter = [&type]()
		{
			switch(type)
			{
				case Type::Anisotropic:
				return D3D11_FILTER_ANISOTROPIC;
				case Type::Bilinear:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				case Type::Point:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			}
		}();
		sDesc.AddressU = isReflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressV = isReflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

		GFX_THROW_INFO(GetDevice(graphics)->CreateSamplerState(&sDesc, &pSampler));
	}

	void Sampler::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->PSSetSamplers(0, 1, pSampler.GetAddressOf()));
	}

	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& graphics, Type type, bool isReflect)
	{
		return Codex::Resolve<Sampler>(graphics, type, isReflect);
	}

	std::string Sampler::GenerateUID(Type type, bool isReflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + std::to_string(static_cast<int>(type)) + (isReflect ? "R"s : "W"s);
	}

	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(type, isReflect);
	}
}