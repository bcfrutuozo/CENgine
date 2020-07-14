#pragma once

#include "Frustum.h"

#include <DirectXMath.h>

class Graphics;

namespace RGP
{
	class RenderGraph;
}

class Projection
{
public:

	Projection(Graphics& graphics, float width, float height, float nearZ, float farZ);
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets(Graphics& graphics);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void Submit(size_t channel) const;
	void LinkTechniques(RGP::RenderGraph& renderGraph);
	void Reset(Graphics& graphics);
private:

	float width;
	float height;
	float nearZ;
	float farZ;
	float homeWidth;
	float homeHeight;
	float homeNearZ;
	float homeFarZ;
	Frustum frustum;
};

