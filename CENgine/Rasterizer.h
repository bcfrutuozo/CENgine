#pragma once

#include "Bindable.h"

#include <array>

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:

		Rasterizer(Graphics& graphics, const bool isTwoSided);
		void Bind(Graphics& graphics) noexcept override;
		static std::shared_ptr<Rasterizer> Resolve(Graphics& graphics, const bool isTwoSided);
		static std::string GenerateUID(const bool isTwoSided);
		std::string GetUID() const noexcept override;
	protected:

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		bool isTwoSided;
	};
}

