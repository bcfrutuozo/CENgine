#pragma once

#include "Bindable.h"
#include "BufferResource.h"

class Graphics;

namespace Bind
{
	class RenderTarget;

	class DepthStencil : public Bindable, public BufferResource
	{
		friend RenderTarget;
	public:

		void BindAsBuffer(Graphics& graphics) noexcept override;
		void BindAsBuffer(Graphics& graphics, BufferResource* renderTarget) noexcept override;
		void BindAsBuffer(Graphics& graphics, RenderTarget* renderTarget) noexcept;
		void Clear(Graphics& graphics) noexcept override;
	protected:

		DepthStencil(Graphics& graphics, UINT width, UINT height, bool canBindShaderInput);

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:

		ShaderInputDepthStencil(Graphics& graphics, UINT slot);
		ShaderInputDepthStencil(Graphics& graphics, UINT width, UINT height, UINT slot);
		void Bind(Graphics& graphics) noexcept override;
	private:

		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:

		OutputOnlyDepthStencil(Graphics& graphics);
		OutputOnlyDepthStencil(Graphics&, UINT width, UINT height);
		void Bind(Graphics& graphics) noexcept override;
	};
}
