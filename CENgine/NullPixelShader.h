#pragma once

#include "Bindable.h"

namespace Bind
{
	class NullPixelShader : public Bindable
	{
	public:

		NullPixelShader(Graphics& graphics);
		void Bind(Graphics& graphics) NOXND override;
		static std::shared_ptr<NullPixelShader> Resolve(Graphics& graphics);
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	};
}