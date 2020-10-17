#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"
#include "Surface.h"

#include <array>

namespace Bind
{
	RenderTarget::RenderTarget(Graphics& graphics, UINT width, UINT height)
		:
		width(width),
		height(height)
	{
		INFOMAN(graphics);

		// Create texture resource
		D3D11_TEXTURE2D_DESC t2dDesc = { };

		t2dDesc.Width = width;
		t2dDesc.Height = height;
		t2dDesc.MipLevels = 1;
		t2dDesc.ArraySize = 1;
		t2dDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		t2dDesc.SampleDesc.Count = 1;
		t2dDesc.SampleDesc.Quality = 0;
		t2dDesc.Usage = D3D11_USAGE_DEFAULT;
		t2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		t2dDesc.CPUAccessFlags = 0;
		t2dDesc.MiscFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(
			&t2dDesc, nullptr, &pTexture));

		// Create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = { };
		rtvDesc.Format = t2dDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		GFX_THROW_INFO(GetDevice(graphics)->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &pTargetView));
	}

	RenderTarget::RenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture, std::optional<UINT> face)
	{
		INFOMAN(graphics);

		// Get information from texture about dimensions
		D3D11_TEXTURE2D_DESC t2dDesc;
		pTexture->GetDesc(&t2dDesc);
		width = t2dDesc.Width;
		height = t2dDesc.Height;

		// Create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = t2dDesc.Format;
		
		if (face.has_value())
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.ArraySize = 1;
			rtvDesc.Texture2DArray.FirstArraySlice = *face;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		}

		GFX_THROW_INFO(GetDevice(graphics)->CreateRenderTargetView(
			pTexture, &rtvDesc, &pTargetView));
	}

	void RenderTarget::BindAsBuffer(Graphics& graphics) NOXND
	{
		ID3D11DepthStencilView* const null = nullptr;
		BindAsBuffer(graphics, null);
	}

	void RenderTarget::BindAsBuffer(Graphics& graphics, BufferResource* depthStencil) NOXND
	{
		assert(dynamic_cast<DepthStencil*>(depthStencil) != nullptr);
		BindAsBuffer(graphics, static_cast<DepthStencil*>(depthStencil));
	}

	void RenderTarget::BindAsBuffer(Graphics& graphics, DepthStencil* depthStencil) NOXND
	{
		BindAsBuffer(graphics, depthStencil ? depthStencil->pDepthStencilView.Get() : nullptr);
	}

	void RenderTarget::BindAsBuffer(Graphics& graphics, ID3D11DepthStencilView* pDepthStencilView) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), pDepthStencilView));

		// Configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<float>(width);
		vp.Height = static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		GFX_THROW_INFO_ONLY(GetContext(graphics)->RSSetViewports(1u, &vp));
	}

	void RenderTarget::Clear(Graphics& graphics, const std::array<float, 4>& color) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->ClearRenderTargetView(pTargetView.Get(), color.data()));
	}

	void RenderTarget::Clear(Graphics& graphics) NOXND
	{
		Clear(graphics, { 0.0f, 0.0f, 0.0f, 0.0f });
	}

	UINT RenderTarget::GetWidth() const noexcept
	{
		return width;
	}

	UINT RenderTarget::GetHeight() const noexcept
	{
		return height;
	}

	ShaderInputRenderTarget::ShaderInputRenderTarget(Graphics& graphics, UINT width, UINT height, UINT slot)
		:
		RenderTarget(graphics, width, height),
		slot(slot)
	{
		INFOMAN(graphics);

		Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
		pTargetView->GetResource(&pResource);

		// Create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO( GetDevice( graphics )->CreateShaderResourceView(
			pResource.Get(),&srvDesc,&pShaderResourceView
		) );
	}

	void ShaderInputRenderTarget::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->PSSetShaderResources(slot, 1, pShaderResourceView.GetAddressOf()));
	}

	Surface Bind::ShaderInputRenderTarget::ToSurface(Graphics& graphics) const
	{
		INFOMAN(graphics);

		// Creating a temp texture compatible with the source but with CPU read access
		Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
		pShaderResourceView->GetResource(&pResource);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureSource;
		pResource.As(&pTextureSource);

		D3D11_TEXTURE2D_DESC tDesc;
		pTextureSource->GetDesc(&tDesc);
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tDesc.Usage = D3D11_USAGE_STAGING;
		tDesc.BindFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureTemp;
		GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(&tDesc, nullptr, &pTextureTemp));

		// Copy texture contents
		GFX_THROW_INFO_ONLY(GetContext(graphics)->CopyResource(pTextureTemp.Get(), pTextureSource.Get()));

		// Create surface and copy from temp texture to it
		const auto width = GetWidth();
		const auto height = GetHeight();

		Surface s{width, height};
		D3D11_MAPPED_SUBRESOURCE msr = { };
		GFX_THROW_INFO(GetContext(graphics)->Map(pTextureTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr));

		auto pSourceBytes = static_cast<const char*>(msr.pData);
		for(unsigned int y = 0; y < height; y++)
		{
			auto pSourceRow = reinterpret_cast<const Surface::Color*>(pSourceBytes + (msr.RowPitch * size_t(y)));
			for(unsigned int x = 0; x < width; x++)
			{
				s.PutPixel(x, y, *(pSourceRow + x));
			}
		}

		GFX_THROW_INFO_ONLY(GetContext(graphics)->Unmap(pTextureTemp.Get(), 0));

		return s;
	}

	void OutputOnlyRenderTarget::Bind(Graphics& graphics) NOXND
	{
		assert("Cannot bind OutputOnlyRenderTarget as shader input" && false);
	}

	OutputOnlyRenderTarget::OutputOnlyRenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture, std::optional<UINT> face)
		:
		RenderTarget(graphics, pTexture, face)
	{ }
}