#pragma once

#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:

		enum class Type
		{
			Anisotropic,
			Bilinear,
			Point,
		};

		Sampler(Graphics& graphics, Type type, bool isReflect);
		void Bind(Graphics& graphics) NOXND override;
		static std::shared_ptr<Sampler> Resolve(Graphics& graphics, Type type = Type::Anisotropic, bool isReflect = false);
		static std::string GenerateUID(Type type, bool isReflect);
		std::string GetUID() const noexcept override;
	protected:

		Type type;
		bool isReflect;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}