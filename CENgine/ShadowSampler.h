#pragma once

#include "Bindable.h"

namespace Bind
{
	class ShadowSampler : public Bindable
	{
	public:

		ShadowSampler(Graphics& graphics);
		void Bind(Graphics& graphics) NOXND override;
		void SetBilinear(bool isBilinear);
		void SetHardwarePcf(bool hasHardwarePcf);
		bool IsBilinear() const;
		bool HasHardwarePcf() const;
	protected:

		size_t currentSampler;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[4];
	private:

		UINT GetCurrentSlot() const;
		static size_t ShadowSamplerIndex(bool isBilinear, bool hasHardwarePcf);
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> MakeSampler(Graphics& graphics, bool isBilinear, bool hasHardwarePcf);
	};
}