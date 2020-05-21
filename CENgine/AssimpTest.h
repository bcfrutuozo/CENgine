#pragma once

#include "Object.h"
#include "ConstantBuffer.h"

class AssimpTest : public Object<AssimpTest>
{
public:

	AssimpTest(Graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		DirectX::XMFLOAT3 material,
		float scale );
};

