#include "CubeTexture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

#include <vector>

namespace Bind
{
	CubeTexture::CubeTexture(Graphics& graphics, const std::string& path, UINT slot)
		:
		path(path),
		slot(slot)
	{
		INFOMAN(graphics);

		// load 6 surfaces for cube faces
		std::vector<Surface> surfaces;
		for(int i = 0; i < 6; i++)
		{
			surfaces.push_back(Surface::FromFile(path + "\\" + std::to_string(i) + ".png"));
		}

		// texture descriptor
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = surfaces[0].GetWidth();
		tDesc.Height = surfaces[0].GetHeight();
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 6;
		tDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.CPUAccessFlags = 0;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		// subresource data
		D3D11_SUBRESOURCE_DATA data[6];
		for(int i = 0; i < 6; i++)
		{
			data[i].pSysMem = surfaces[i].GetBufferPtrConst();
			data[i].SysMemPitch = surfaces[i].GetBytePitch();
			data[i].SysMemSlicePitch = 0;
		}
		// create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(graphics)->CreateTexture2D(
			&tDesc, data, &pTexture
		));

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(graphics)->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		));
	}

	void CubeTexture::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf()));
	}
}