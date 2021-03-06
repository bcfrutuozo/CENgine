#pragma once

#include "Bindable.h"

namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		
		VertexShader(Graphics& graphics, const std::string& path);
		void Bind(Graphics& graphics) NOXND override;
		ID3DBlob* GetByteCode() const noexcept;
		static std::shared_ptr<VertexShader> Resolve(Graphics& graphics, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
	protected:

		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> pByteCodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	};
}