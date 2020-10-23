#pragma once

#include "GPU.h"

class AmdGPU : public GPU
{
public:

	AmdGPU(Device device);
	~AmdGPU();
	bool Initialize() override;
	void GetWorkload() override;
	
private:
};