#pragma once

#include "Bindable.h"

namespace Bind
{
	class ShadowSampler : public Bindable
	{
	public:
		
		ShadowSampler(Graphics& graphics);
		void Bind(Graphics& graphics) NOXND override;
	protected:

		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}