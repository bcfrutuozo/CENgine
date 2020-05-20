#pragma once

#include "Bindable.h"

class Texture : public Bindable
{
public:
	
	Texture(Graphics& graphics, const class Surface& surface);
	void Bind(Graphics& graphics) noexcept override;
protected:

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};

