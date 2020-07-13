#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	DepthStencil::DepthStencil(Graphics& graphics, UINT width, UINT height, bool canBindShaderInput)
	{
		INFOMAN(graphics);

		// Create depth stencil texture
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilTexture;
		D3D11_TEXTURE2D_DESC desc = { };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// This will need to be fixed!
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);

		GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(&desc, nullptr, &pDepthStencilTexture));

		// Create target view of depth stensil texture
		GFX_THROW_INFO(GetDevice(graphics)->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, &pDepthStencilView)); //nullptr will need to be replaced
	}

	void DepthStencil::BindAsBuffer(Graphics& graphics) noexcept
	{
		GetContext(graphics)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
	}

	void DepthStencil::BindAsBuffer(Graphics& graphics, BufferResource* renderTarget) noexcept
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(graphics, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindAsBuffer(Graphics& graphics, RenderTarget* renderTarget) noexcept
	{
		renderTarget->BindAsBuffer(graphics, this);
	}

	void DepthStencil::Clear(Graphics& graphics) noexcept
	{
		GetContext(graphics)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& graphics, UINT slot)
		:
		ShaderInputDepthStencil(graphics, graphics.GetWidth(), graphics.GetHeight(), slot)
	{ }

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& graphics, UINT width, UINT height, UINT slot)
		:
		DepthStencil(graphics, width, height, true),
		slot(slot)
	{
		INFOMAN(graphics);

		Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
		pDepthStencilView->GetResource(&pResource);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // this will need to be fixed
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(graphics)->CreateShaderResourceView(
			pResource.Get(), &srvDesc, &pShaderResourceView
		));
	}

	void ShaderInputDepthStencil::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->PSSetShaderResources(slot, 1u, pShaderResourceView.GetAddressOf());
	}


	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics)
		:
		OutputOnlyDepthStencil(graphics, graphics.GetWidth(), graphics.GetHeight())
	{ }

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& graphics, UINT width, UINT height)
		:
		DepthStencil(graphics, width, height, false)
	{ }

	void OutputOnlyDepthStencil::Bind(Graphics& graphics) noexcept
	{
		assert("OutputOnlyDepthStencil cannot be bound as shader input" && false);
	}
}