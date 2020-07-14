#pragma once

#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffer.h"
#include "Conditional_noexcept.h"

namespace RGP
{
	class RenderGraph;
}

class Camera;

class PointLight
{
public:

	PointLight(Graphics& graphics, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit(size_t channels) const NOXND;
	void Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept;
	void LinkTechniques(RGP::RenderGraph& renderGraph);
	std::shared_ptr<Camera> ShareCamera() const noexcept;
private:

	struct PointLightCBuf
	{
		/* alignas is used to pad bytes for the pixel shader */
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};

	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> constantBuffer;
	std::shared_ptr<Camera> pCamera;
};

