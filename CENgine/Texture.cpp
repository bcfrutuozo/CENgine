#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {

	Texture::Texture(Graphics& graphics, const std::string& path, UINT slot)
		:
		path(path),
		slot(slot)
	{
		INFOMAN(graphics);

		// Load surface
		const auto surface = Surface::FromFile(path);
		
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
		GetContext(graphics)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& graphics, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>(graphics, path, slot);
	}

	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + std::to_string(slot);
	}

	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(path, slot);
	}
}