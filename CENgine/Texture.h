#pragma once

#include "Bindable.h"

class Surface;

namespace Bind {

	class Texture : public Bindable
	{
	public:

		Texture(Graphics& graphics, const std::string& path, UINT slot = 0);
		void Bind(Graphics& graphics) noexcept override;
		static std::shared_ptr<Texture> Resolve(Graphics& graphics, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
		std::string GetUID() const noexcept override;
		bool HasAlpha() const noexcept;
	protected:

		bool hasAlpha = false;
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	private:

		static UINT CalculateNumberOfMipLevels(const UINT width, const UINT height) noexcept;

		unsigned int slot;
	};
}