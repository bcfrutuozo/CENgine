#pragma once

#include "Bindable.h"

class Sampler : public Bindable
{
public:

	Sampler(Graphics& graphics);
	void Bind(Graphics& graphics) noexcept override;
protected:

	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
};

