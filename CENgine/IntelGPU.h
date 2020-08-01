#pragma once

#include "GPU.h"

class IntelGPU : public GPU
{
public:

	IntelGPU(Device device);
	~IntelGPU();
	void Initialize() override;
	void GetWorkload() override;
	
private:
};