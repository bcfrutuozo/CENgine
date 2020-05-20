#pragma once

#include "Object.h"

class Cylinder : public Object<Cylinder>
{
public:

	Cylinder(Graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& bDist,
		std::uniform_int_distribution<int>& tDist);
};

