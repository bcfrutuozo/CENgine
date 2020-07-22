#pragma once

#include "Bindable.h"

#include <array>

namespace Bind
{
	class ShadowRasterizer : public Bindable
	{
	public:

		ShadowRasterizer(Graphics& graphics, int depthBias, float slopeBias, float clamp);
		void ChangeDepthBiasParameters(Graphics& graphics, int depthBias_in, float slopeBias_in, float clamp_in);
		void Bind(Graphics& graphics) NOXND override;
		int GetDepthBias() const;
		float GetSlopeBias() const;
		float GetClamp() const;
	protected:

		int depthBias;
		float slopeBias;
		float clamp;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	};
}