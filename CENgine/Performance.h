#pragma once

#include "GPU.h"
#include "Timer.h"

#include <Pdh.h>
#include <Psapi.h>

#pragma comment(lib, "pdh.lib")


class Performance
{
public:

	Performance(Timer& timer);
	void Initialize();
	void Shutdown();
	void ShowWidget();
private:

	void Calculate();
	void GetCPUTotalWorkload();
	void GetCPUEngineWorkload();
	void GetCPUTemperature();
	void GetMemorySizeInformation();
	void GetMemoryTotalUsage();
	void GetMemoryEngineUsage();

	static constexpr float updatePeriod = 0.5f;

	// Variables to check total CPU workload
	bool canReadCpu;
	HQUERY queryHandle;
	HCOUNTER counterHandle;

	// Variables to check the engine workload on the CPU
	ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	DWORD numberOfProcessors;
	HANDLE self;

	// Timer to avoid performace retrieval on every frame
	Timer timer;
	float lastSampleTime;

	// Variables to check memory
	MEMORYSTATUSEX memInfo;
	PROCESS_MEMORY_COUNTERS_EX pmc;

	std::unique_ptr<GPU> m_GPU;
	float m_CPUTotalWorkload;
	float m_CPUEngineWorkload;
	long m_CPUTemperature;
	long m_MemoryLoad;
	unsigned long m_TotalPhysicalMemory;
	unsigned long m_TotalVirtualMemory;
	unsigned long m_VirtualMemoryTotalWorkload;
	unsigned long m_VirtualMemoryEngineWorkload;
	unsigned long m_PhysicalMemoryTotalWorkload;
	unsigned long m_PhysicalMemoryEngineWorkload;
};