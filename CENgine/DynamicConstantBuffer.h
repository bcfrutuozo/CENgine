#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "DynamicConstant.h"

namespace Bind
{
	class DynamicPixelConstantBuffer : public Bindable
	{
	public:

		void Update(Graphics& graphics, const DRR::Buffer& buffer)
		{
			assert(&buffer.GetRootLayoutElement() == &GetRootLayoutElement());
			INFOMAN(graphics);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(graphics)->Map(
				pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, buffer.GetData(), buffer.GetSizeInBytes());
			GetContext(graphics)->Unmap(pConstantBuffer.Get(), 0u);
		}

		void Bind(Graphics& graphics) noexcept override
		{
			GetContext(graphics)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}

		virtual const DRR::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:

		DynamicPixelConstantBuffer(Graphics& graphics, const DRR::LayoutElement& layoutRoot, UINT slot, const DRR::Buffer* pBuffer)
			:
			slot(slot)
		{
			INFOMAN(graphics);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if(pBuffer != nullptr)
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuffer->GetData();
				GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
			}
			else
			{
				GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
			}
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	class DynamicCachingPixelConstantBuffer  : public DynamicPixelConstantBuffer
	{
	public:
		DynamicCachingPixelConstantBuffer(Graphics& graphics, const DRR::CompleteLayout& layout, UINT slot)
			:
			DynamicPixelConstantBuffer(graphics, *layout.ShareRoot(), slot, nullptr),
			buffer(DRR::Buffer(layout))
		{ }

		DynamicCachingPixelConstantBuffer(Graphics& graphics, const DRR::Buffer& buffer, UINT slot)
			:
			DynamicPixelConstantBuffer(graphics, buffer.GetRootLayoutElement(), slot, &buffer),
			buffer(buffer)
		{ }

		const DRR::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return buffer.GetRootLayoutElement();
		}

		const DRR::Buffer& GetBuffer() const noexcept
		{
			return buffer;
		}

		void SetBuffer(const DRR::Buffer& buffer_in)
		{
			buffer.CopyFrom(buffer_in);
			dirty = true;
		}

		void Bind(Graphics& graphics) noexcept override
		{
			if(dirty)
			{
				Update(graphics, buffer);
				dirty = false;
			}

			DynamicPixelConstantBuffer::Bind(graphics);
		}
	private:

		bool dirty = false;
		DRR::Buffer buffer;
	};

	class DynamicNoCachePixelConstantBuffer : public DynamicPixelConstantBuffer
	{
	public:

		DynamicNoCachePixelConstantBuffer(Graphics& graphics, const DRR::CompleteLayout& layout, UINT slot)
			:
			DynamicPixelConstantBuffer(graphics, *layout.ShareRoot(), slot, nullptr),
			pLayoutRoot(layout.ShareRoot())
		{ }

		DynamicNoCachePixelConstantBuffer(Graphics& graphics, const DRR::Buffer& buffer, UINT slot)
			:
			DynamicPixelConstantBuffer(graphics, buffer.GetRootLayoutElement(), slot, &buffer),
			pLayoutRoot(buffer.ShareLayoutRoot())
		{ }

		const DRR::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return *pLayoutRoot;
		}
	private:

		std::shared_ptr<DRR::LayoutElement> pLayoutRoot;
	};
}