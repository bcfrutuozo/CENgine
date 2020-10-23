#pragma once

#include "CENgineWin.h"
#include "Peripheral.h"
#include "Core.h"

class CPU : public Peripheral
{
public:

	CPU(std::vector<std::unique_ptr<Core>>&& cores);
	~CPU();
	bool Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
protected:

	// Variables to check total CPU workload
	bool canReadCPU;
	HQUERY queryHandle;
	HCOUNTER counterHandle;

	// Variables to check the engine workload on the CPU
	ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	DWORD numberOfProcessors;
	HANDLE self;

	std::vector<std::unique_ptr<Core>> m_Cores;
	float m_CPUTotalWorkload;
	float m_CPUEngineWorkload;
};

