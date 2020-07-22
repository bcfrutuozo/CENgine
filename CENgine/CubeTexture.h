#pragma once

#include "Bindable.h"

class Surface;

namespace Bind
{
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
}