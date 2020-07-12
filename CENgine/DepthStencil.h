#pragma once

#include "Graphics.h"
#include "GraphicsResource.h"

class DepthStencil : public GraphicsResource
{
	friend class RenderTarget;
	friend class Graphics;
public:

	DepthStencil(Graphics& graphics, UINT width, UINT height);
	void BindAsDepthStencil(Graphics& graphics) const noexcept;
	void Clear(Graphics& graphics) const noexcept;
private:

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

