#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

Texture::Texture(Graphics& graphics, const Surface& surface)
{
	INFOMAN(graphics);

	// Create the texture resource
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = surface.GetWidth();
	texDesc.Height = surface.GetHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = surface.GetBufferPtr();
	sd.SysMemPitch = surface.GetWidth() * sizeof(Surface::Color);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

	GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(&texDesc, &sd, &pTexture));

	// Create the texture resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_THROW_INFO(GetDevice(graphics)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
}

void Texture::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
}