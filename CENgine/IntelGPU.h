#pragma once

#include "GPU.h"

class IntelGPU : public GPU
{
public:

	IntelGPU(Device device);
	~IntelGPU();
	bool Initialize() override;
	void GetWorkload() override;
	
private:
};