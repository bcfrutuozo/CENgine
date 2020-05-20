#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(Graphics& graphics, const std::wstring& path)
{
	INFOMAN(graphics);

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pByteCodeBlob));
	GFX_THROW_INFO(GetDevice(graphics)->CreateVertexShader(
		pByteCodeBlob->GetBufferPointer(),
		pByteCodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader));
}

void VertexShader::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pByteCodeBlob.Get();
}