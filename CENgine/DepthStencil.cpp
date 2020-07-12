#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"

DepthStencil::DepthStencil(Graphics& graphics, UINT width, UINT height)
{
	INFOMAN(graphics);

	// Create depth stencil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilTexture;
	D3D11_TEXTURE2D_DESC desc = { };
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1u;
	desc.SampleDesc.Quality = 0u;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(&desc, nullptr, &pDepthStencilTexture));

	// Create target view of depth stensil texture
	GFX_THROW_INFO(GetDevice(graphics)->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, &pDepthStencilView));
}

void DepthStencil::BindAsDepthStencil(Graphics& graphics) const noexcept
{
	GetContext(graphics)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
}

void DepthStencil::Clear(Graphics& graphics) const noexcept
{
	GetContext(graphics)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}
