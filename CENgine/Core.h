#pragma once

#include "CENgineWin.h"
#include "Peripheral.h"

#include <Pdh.h>
#include <Psapi.h>

class Core : public Peripheral
{
public:

	Core(Device device);
	~Core();
	void Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
private:

	HQUERY hQuery;
	HCOUNTER hIdleCounters;
	HCOUNTER hPrivelegedCounters;
	HCOUNTER hProcessorCounters;

	float m_Idle;
	float m_Kernel;
	float m_Processor;
};

