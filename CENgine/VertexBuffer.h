#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& graphics, const std::vector<V>& vertices)
		:
	stride(sizeof(V))
	{
		INFOMAN(graphics);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(V) * vertices.size());
		bd.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA isResourceData = {};
		isResourceData.pSysMem = vertices.data();
		GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&bd, &isResourceData, &pVertexBuffer));
	}

	void Bind(Graphics& graphics) noexcept override;

protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

