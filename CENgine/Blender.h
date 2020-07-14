#pragma once

#include "Bindable.h"

#include <array>
#include <optional>

namespace Bind
{
	class Blender : public Bindable
	{
	public:

		Blender(Graphics& graphics, bool isBlending, std::optional<float> factors_in = {});
		void Bind(Graphics& graphics) NOXND override;
		void SetFactor(const float factor) NOXND;
		float GetFactor() const NOXND;
		static std::shared_ptr<Blender> Resolve(Graphics& graphics, bool isBlending, std::optional<float> factor = {});
		static std::string GenerateUID(bool isBlending, std::optional<float> factor);
		std::string GetUID() const noexcept override;
	protected:

		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
		bool isBlending;
		std::optional<std::array<float, 4>> factors;
	};
}