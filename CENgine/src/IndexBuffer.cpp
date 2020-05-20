#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"

IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<unsigned short>& indices)
	:
	count((UINT)indices.size())
{
	INFOMAN(graphics);

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isrd = {};
	isrd.pSysMem = indices.data();

	GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&ibd, &isrd, &pIndexBuffer));	
}

void IndexBuffer::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}