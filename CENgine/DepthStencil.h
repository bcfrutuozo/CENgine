#pragma once

#include "Bindable.h"
#include "BufferResource.h"
#include "Surface.h"

class Graphics;

namespace Bind
{
	class RenderTarget;

	class DepthStencil : public Bindable, public BufferResource
	{
		friend RenderTarget;
	public:

		enum class Usage
		{
			DepthStencil,
			ShadowDepth,
		};

		void BindAsBuffer(Graphics& graphics) NOXND override;
		void BindAsBuffer(Graphics& graphics, BufferResource* renderTarget) NOXND override;
		void BindAsBuffer(Graphics& graphics, RenderTarget* renderTarget) NOXND;
		void Clear(Graphics& graphics) NOXND override;
		Surface ToSurface(Graphics& graphics, bool linearize = true) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	protected:

		DepthStencil(Graphics& graphics, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face);
		DepthStencil(Graphics& graphics, UINT width, UINT height, bool canBindShaderInput, Usage usage);

		unsigned int width;
		unsigned int height;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:

		ShaderInputDepthStencil(Graphics& graphics, UINT slot, Usage usage = Usage::DepthStencil);
		ShaderInputDepthStencil(Graphics& graphics, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil);
		void Bind(Graphics& graphics) NOXND override;
	private:

		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:

		OutputOnlyDepthStencil(Graphics& graphics, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face);
		OutputOnlyDepthStencil(Graphics& graphics);
		OutputOnlyDepthStencil(Graphics&, UINT width, UINT height);
		void Bind(Graphics& graphics) NOXND override;
	};
}
