#pragma once

#include "Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		
		PixelShader(Graphics& graphics, const std::string& path);
		void Bind(Graphics& graphics) NOXND override;
		static std::shared_ptr<PixelShader> Resolve(Graphics& graphics, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
	protected:

		std::string path;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
}