#pragma once

#include "Bindable.h"
#include "BufferResource.h"

class Graphics;

namespace Bind
{
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource
	{
	public:

		void BindAsBuffer(Graphics& graphics) noexcept override;
		void BindAsBuffer(Graphics& graphics, BufferResource* depthStencil) noexcept override;
		void BindAsBuffer(Graphics& graphics, DepthStencil* depthStencil) noexcept;
		void Clear(Graphics& graphics) noexcept override;
		void Clear(Graphics& graphics, const std::array<float, 4>& color) noexcept;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
	protected:

		RenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture);
		RenderTarget(Graphics& graphics, UINT width, UINT height);

		UINT width;
		UINT height;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	private:

		void BindAsBuffer(Graphics& graphics, ID3D11DepthStencilView* pDepthStencilView) noexcept;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:

		ShaderInputRenderTarget(Graphics& graphics, UINT width, UINT height, UINT slot);
		void Bind(Graphics& graphics) noexcept override;
	private:

		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	// RenderTarget for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend Graphics;

	public:

		void Bind(Graphics& graphics) noexcept override;
	private:

		OutputOnlyRenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture);
	};
}