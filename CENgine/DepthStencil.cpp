#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "Surface.h"

#include <stdexcept>

namespace Bind
{
	DXGI_FORMAT MapUsageTypeless(DepthStencil::Usage usage)
	{
		switch(usage)
		{
			case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
			case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		}
		throw std::runtime_error{ "Base usage for Typeless format map in DepthStencil." };
	}

	DXGI_FORMAT MapUsageTyped(DepthStencil::Usage usage)
	{
		switch(usage)
		{
			case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		}
		throw std::runtime_error{ "Base usage for Typed format map in DepthStencil." };
	}

	DXGI_FORMAT MapUsageColored(DepthStencil::Usage usage)
	{
		switch(usage)
		{
			case DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		}
		throw std::runtime_error{ "Base usage for Colored format map in DepthStencil." };
	}

	DepthStencil::DepthStencil(Graphics& graphics, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face)
	{
		INFOMAN(graphics);

		D3D11_TEXTURE2D_DESC textureDesc = {};
		pTexture->GetDesc(&textureDesc);
		width = textureDesc.Width;
		height = textureDesc.Height;

		// Create target view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		viewDesc.Flags = 0;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MipSlice = 0;
		viewDesc.Texture2DArray.ArraySize = 1;
		viewDesc.Texture2DArray.FirstArraySlice = face;
		GFX_THROW_INFO(GetDevice(graphics)->CreateDepthStencilView(pTexture.Get(), &viewDesc, &pDepthStencilView));	
	}

	DepthStencil::DepthStencil(Graphics& graphics, UINT width, UINT height, bool canBindShaderInput, Usage usage)
		:
		width(width),
		height(height)
	{
		INFOMAN(graphics);

		// Create depth stencil texture
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilTexture;
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.Format = MapUsageTypeless(usage);
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);

		GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(&desc, nullptr, &pDepthStencilTexture));

		// Create target view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = MapUsageTyped(usage);
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;

		// Create target view of depth stencil texture
		GFX_THROW_INFO(GetDevice(graphics)->CreateDepthStencilView(pDepthStencilTexture.Get(), &descView, &pDepthStencilView)); //nullptr will need to be replaced
	}

	void DepthStencil::BindAsBuffer(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get()));
	}

	void DepthStencil::BindAsBuffer(Graphics& graphics, BufferResource* renderTarget) NOXND
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(graphics, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindAsBuffer(Graphics& graphics, RenderTarget* renderTarget) NOXND
	{
		renderTarget->BindAsBuffer(graphics, this);
	}

	void DepthStencil::Clear(Graphics& graphics) NOXND
	{
		GetContext(graphics)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	unsigned int DepthStencil::GetWidth() const
	{
		return width;
	}

	Surface Bind::DepthStencil::ToSurface(Graphics& graphics, bool linearize) const
	{
		INFOMAN(graphics);

		// Get info about the stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC sourceViewDesc{};
		pDepthStencilView->GetDesc(&sourceViewDesc);

		// Creating a temp texture compatible with the source, but with CPU read access
		Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
		pDepthStencilView->GetResource(&pResource);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexSource;
		pResource.As(&pTexSource);
		
		D3D11_TEXTURE2D_DESC sourceTextureDesc{};
		pTexSource->GetDesc(&sourceTextureDesc);
		D3D11_TEXTURE2D_DESC tmpTextureDesc = sourceTextureDesc;
		tmpTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tmpTextureDesc.Usage = D3D11_USAGE_STAGING;
		tmpTextureDesc.BindFlags = 0;
		tmpTextureDesc.MiscFlags = 0;
		tmpTextureDesc.ArraySize = 1;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexTemp;
		GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(
			&tmpTextureDesc, nullptr, &pTexTemp
		));

		// copy texture contents
		if(sourceViewDesc.ViewDimension == D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
		{
			// Source is actually inside a cubemap texture, use view info to find the correct slice and copy the subresource
			GFX_THROW_INFO_ONLY(GetContext(graphics)->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), sourceViewDesc.Texture2DArray.FirstArraySlice, nullptr));
		}
		else
		{
			GFX_THROW_INFO_ONLY(GetContext(graphics)->CopyResource(pTexTemp.Get(), pTexSource.Get()));
		}

		// create Surface and copy from temp texture to it
		const auto width = GetWidth();
		const auto height = GetHeight();
		Surface s{ width, height };
		D3D11_MAPPED_SUBRESOURCE msr = {};
		GFX_THROW_INFO(GetContext(graphics)->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr));
		auto pSrcBytes = static_cast<const char*>(msr.pData);
		for(unsigned int y = 0; y < height; y++)
		{
			struct Pixel
			{
				char data[4];
			};
			auto pSrcRow = reinterpret_cast<const Pixel*>(pSrcBytes + msr.RowPitch * size_t(y));
			for(unsigned int x = 0; x < width; x++)
			{
				if(sourceTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS)
				{
					const auto raw = 0xFFFFFF & *reinterpret_cast<const unsigned int*>(pSrcRow + x);
					if(linearize)
					{
						const auto normalized = (float)raw / (float)0xFFFFFF;
						const auto linearized = 0.01f / (1.01f - normalized);
						const auto channel = unsigned char(linearized * 255.0f);
						s.PutPixel(x, y, { channel, channel, channel });
					}
					else
					{
						const unsigned char channel = raw >> 16;
						s.PutPixel(x, y, { channel, channel, channel });
					}
				}
				else if(sourceTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
				{
					const auto raw = *reinterpret_cast<const float*>(pSrcRow + x);
					if(linearize)
					{
						const auto linearized = 0.01f / (1.01f - raw);
						const auto channel = unsigned char(linearized * 255.0f);
						s.PutPixel(x, y, { channel, channel, channel });
					}
					else
					{
						const auto channel = unsigned char(raw * 255.0f);
						s.PutPixel(x, y, { channel, channel, channel });
					}
				}
				else
				{
					throw std::runtime_error{ "Bad format in Depth Stencil for conversion to Surface" };
				}
			}
		}
		GFX_THROW_INFO_ONLY(GetContext(graphics)->Unmap(pTexTemp.Get(), 0));

		return s;
	}

	unsigned int DepthStencil::GetHeight() const
	{
		return height;
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& graphics, UINT slot, Usage usage)
		:
		ShaderInputDepthStencil(graphics, graphics.GetWidth(), graphics.GetHeight(), slot, usage)
	{}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& graphics, UINT width, UINT height, UINT slot, Usage usage)
		:
		DepthStencil(graphics, width, height, true, usage),
		slot(slot)
	{
		INFOMAN(graphics);

		Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
		pDepthStencilView->GetResource(&pResource);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = MapUsageColored(usage);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(graphics)->CreateShaderResourceView(
			pResource.Get(), &srvDesc, &pShaderResourceView
		));
	}

	void ShaderInputDepthStencil::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->PSSetShaderResources(slot, 1u, pShaderResourceView.GetAddressOf()));
	}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face)
		:
		DepthStencil(graphics, std::move(pTexture), face)
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics)
		:
		OutputOnlyDepthStencil(graphics, graphics.GetWidth(), graphics.GetHeight())
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics, UINT width, UINT height)
		:
		DepthStencil(graphics, width, height, false, Usage::DepthStencil)
	{}

	void OutputOnlyDepthStencil::Bind(Graphics& graphics) NOXND
	{
		assert("OutputOnlyDepthStencil cannot be bound as shader input" && false);
	}
}