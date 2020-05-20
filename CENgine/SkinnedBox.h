#pragma once

#include "Object.h"

class SkinnedBox : public Object<SkinnedBox>
{
public:

	SkinnedBox( Graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist );
};

