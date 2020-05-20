#pragma once

#include "Object.h"
#include "ConstantBuffer.h"

class Box : public Object<Box>
{
public:
	
	Box(Graphics& graphics, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& bDist,
		DirectX::XMFLOAT3 material);

	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	// Returns false if window is closed
	bool SpawnControlWindow(int id, Graphics& graphics) noexcept;
private:

	void SyncMaterial(Graphics& graphics) noexcept(!IS_DEBUG);

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} materialConstants;
	using MaterialCbuf = PixelConstantBuffer<PSMaterialConstant>;
	
	// Model transform
	DirectX::XMFLOAT3X3 mt;
};

