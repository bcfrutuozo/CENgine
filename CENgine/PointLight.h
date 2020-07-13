#pragma once

#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffer.h"
#include "Conditional_noexcept.h"

class RenderGraph;

class PointLight
{
public:

	PointLight(Graphics& graphics, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit() const NOXND;
	void Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept;
	void LinkTechniques(RenderGraph& renderGraph);
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
};

