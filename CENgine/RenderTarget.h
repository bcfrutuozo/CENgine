#pragma once

#include "Graphics.h"
#include "GraphicsResource.h"

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:

	RenderTarget(Graphics& graphics, UINT width, UINT height);
	void BindAsTexture(Graphics& graphics, UINT slot) const noexcept;
	void BindAsTarget(Graphics& graphics) const noexcept;
	void BindAsTarget(Graphics& graphics, const DepthStencil& depthStencil) const noexcept;
	void Clear(Graphics& graphics, const std::array<float, 4>& color) const noexcept;
	void Clear(Graphics& graphics) const noexcept;
private:

	UINT width;
	UINT height;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
};

