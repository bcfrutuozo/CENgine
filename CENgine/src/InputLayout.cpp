#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

InputLayout::InputLayout(Graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
{
	INFOMAN(graphics);

	GFX_THROW_INFO(GetDevice(graphics)->CreateInputLayout(
	layout.data(), (UINT)layout.size(),
		pVertexShaderByteCode->GetBufferPointer(),
		pVertexShaderByteCode->GetBufferSize(),
		&pInputLayout));
}

void InputLayout::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetInputLayout(pInputLayout.Get());
}