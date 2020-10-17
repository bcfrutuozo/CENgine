#pragma once

#include "Bindable.h"
#include "BufferResource.h"

#include <optional>

class Graphics;
class Surface;

namespace Bind
{
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource
	{
	public:

		void BindAsBuffer(Graphics& graphics) NOXND override;
		void BindAsBuffer(Graphics& graphics, BufferResource* depthStencil) NOXND override;
		void BindAsBuffer(Graphics& graphics, DepthStencil* depthStencil) NOXND;
		void Clear(Graphics& graphics) NOXND override;
		void Clear(Graphics& graphics, const std::array<float, 4>& color) NOXND;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
	protected:

		RenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture, std::optional<UINT> face);
		RenderTarget(Graphics& graphics, UINT width, UINT height);

		UINT width;
		UINT height;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	private:

		void BindAsBuffer(Graphics& graphics, ID3D11DepthStencilView* pDepthStencilView) NOXND;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:

		ShaderInputRenderTarget(Graphics& graphics, UINT width, UINT height, UINT slot);
		void Bind(Graphics& graphics) NOXND override;
		Surface ToSurface(Graphics& graphics) const;
	private:

		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	// RenderTarget for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend Graphics;

	public:

		void Bind(Graphics& graphics) NOXND override;
		OutputOnlyRenderTarget(Graphics& graphics, ID3D11Texture2D* pTexture, std::optional<UINT> face = {});
	};
}