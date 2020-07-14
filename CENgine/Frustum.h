#pragma once

#include "Drawable.h"

class Frustum : public Drawable
{
public:

	Frustum(Graphics& graphics, float width, float height, float nearZ, float farZ);
	void SetVertices(Graphics& graphics, float width, float height, float nearZ, float farZ);
	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rotation) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:

	DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
};