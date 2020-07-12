#pragma once

#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:

		Sampler(Graphics& graphics, bool isAnisoEnabled, bool isReflect);
		void Bind(Graphics& graphics) noexcept override;
		static std::shared_ptr<Sampler> Resolve(Graphics& graphics, bool isAnisoEnabled = true, bool isReflect = false);
		static std::string GenerateUID(bool isAnisoEnabled, bool isReflect);
		std::string GetUID() const noexcept override;
	protected:

		bool isAnisoEnabled;
		bool isReflect;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}