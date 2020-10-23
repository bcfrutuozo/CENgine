#pragma once

#include "Peripheral.h"

class GPU : public Peripheral
{
public:

	GPU(Device device);
	~GPU();
	bool Initialize() override { return true; }
	void ShowWidget() override;
	void GetWorkload() override { }

protected:

	std::string m_DriverInfo;
	size_t m_GPUCount;
	unsigned int m_FanSpeed;
	unsigned int m_Temperature;
	unsigned int m_PowerUsage;
	unsigned int m_GPUUtilization;
	unsigned int m_MemoryUtilization;
};