#pragma once

#include "Bindable.h"

namespace Bind {
	
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& graphics,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
			ID3DBlob* pVertexShaderByteCode);
		void Bind(Graphics& graphics) noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}