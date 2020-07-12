#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"

#include <array>

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
	t2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	t2dDesc.CPUAccessFlags = 0;
	t2dDesc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(
		&t2dDesc, nullptr, &pTexture));

	// Create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
	srvDesc.Format = t2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(GetDevice(graphics)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

	// Create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = { };
	rtvDesc.Format = t2dDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	GFX_THROW_INFO(GetDevice(graphics)->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &pTargetView));
}

void RenderTarget::BindAsTexture(Graphics& graphics, UINT slot) const noexcept
{
	GetContext(graphics)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
}

void RenderTarget::BindAsTarget(Graphics& graphics) const noexcept
{
	GetContext(graphics)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), nullptr);

	// Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GetContext( graphics )->RSSetViewports( 1u,&vp );
}

void RenderTarget::BindAsTarget(Graphics& graphics, const DepthStencil& depthStencil) const noexcept
{
	GetContext(graphics)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), depthStencil.pDepthStencilView.Get());

	// Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GetContext( graphics )->RSSetViewports( 1u,&vp );
}

void RenderTarget::Clear(Graphics& graphics, const std::array<float, 4>& color) const noexcept
{
	GetContext(graphics)->ClearRenderTargetView(pTargetView.Get(), color.data());
}

void RenderTarget::Clear(Graphics & graphics) const noexcept
{
	Clear(graphics, { 0.0f, 0.0f, 0.0f, 0.0f });
}

