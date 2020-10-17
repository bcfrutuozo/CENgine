#pragma once

#include "Bindable.h"

class Surface;

namespace Bind
{
	class OutputOnlyDepthStencil;
	class OutputOnlyRenderTarget;
	
	class CubeTexture : public Bindable
	{
	public:

		CubeTexture(Graphics& graphics, const std::string& path, UINT slot = 0);
		void Bind(Graphics& graphics) NOXND override;
	protected:

		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	private:

		unsigned int slot;
	};

	class CubeTargetTexture : public Bindable
	{
	public:

		CubeTargetTexture(Graphics& graphics, UINT width, UINT height, UINT slot = 0, DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);
		void Bind(Graphics& graphics) NOXND override;
		std::shared_ptr<OutputOnlyRenderTarget> GetRenderTarget(size_t index) const;
	protected:

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::vector<std::shared_ptr<OutputOnlyRenderTarget>> renderTargets;
	private:

		unsigned int slot;
	};

	class DepthCubeTexture : public Bindable
	{
	public:

		DepthCubeTexture(Graphics& graphics, UINT size, UINT slot = 0);
		void Bind(Graphics& graphics) NOXND override;
		std::shared_ptr<OutputOnlyDepthStencil> GetDepthBuffer(size_t index) const;
	protected:

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::vector<std::shared_ptr<OutputOnlyDepthStencil>> depthBuffers;
	private:

		unsigned int slot;
	};
}