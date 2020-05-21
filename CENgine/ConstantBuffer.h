#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

namespace Bind {
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		void Update(Graphics& graphics, const C& consts)
		{
			INFOMAN(graphics);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(graphics)->Map(
				pConstantBuffer.Get(),
				0u,
				D3D11_MAP_WRITE_DISCARD,
				0u,
				&msr));

			memcpy(msr.pData, &consts, sizeof(consts));

			GetContext(graphics)->Unmap(pConstantBuffer.Get(), 0u);
		}

		ConstantBuffer(Graphics& graphics, const C& consts, const UINT slot = 0u)
			:
			slot(slot)
		{
			INFOMAN(graphics);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA cbsrd = {};
			cbsrd.pSysMem = &consts;
			GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&cbd, &cbsrd, &pConstantBuffer));
		}

		ConstantBuffer(Graphics& graphics, const UINT slot = 0u)
			:
			slot(slot)
		{
			INFOMAN(graphics);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;

			GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	protected:

		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;

	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& graphics) noexcept override
		{
			this->GetContext(graphics)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;

	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& graphics) noexcept override
		{
			this->GetContext(graphics)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
}