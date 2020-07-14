#pragma once

#include "Drawable.h"

class CameraIndicator : public Drawable
{
public:

	CameraIndicator(Graphics& graphics);
	void SetPosition(DirectX::XMFLOAT3 position_in) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rotation_in) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:

	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
};

