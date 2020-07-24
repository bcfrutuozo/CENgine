#pragma once

#include "GPU.h"
#include <memory>

class GPU;
class NvidiaGPU;
class AmdGPU;

class Factory
{
public:

	template<typename T>
	static std::unique_ptr<T> Create(Device driver, Manufacturer m = Manufacturer::UNSPECIFIED)
	{
		if(typeid(T) == typeid(GPU))
		{
			if(m == Manufacturer::NVIDIA)
			{
				return std::make_unique<NvidiaGPU>(driver);
			}
			else if(m == Manufacturer::AMD)
			{
				return std::make_unique<AmdGPU>(driver);
			}
			else
			{
				std::runtime_error("Unsupported videocard");
			}
		}

		throw std::runtime_error("Invalid device type");
	}
};