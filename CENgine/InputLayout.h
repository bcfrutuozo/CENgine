#pragma once

#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		
		InputLayout(Graphics& graphics,
			CENgineexp::VertexLayout layout_in,
			ID3DBlob* pVertexShaderByteCode);
		void Bind(Graphics& graphics) noexcept override;
		const CENgineexp::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& graphics, const CENgineexp::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode);
		static std::string GenerateUID(const CENgineexp::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr);
		std::string GetUID() const noexcept override;
	protected:

		CENgineexp::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}