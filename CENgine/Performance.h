#pragma once

#include "Hardware.h"
#include "Timer.h"

class Performance
{
public:

	Performance(Timer& timer);
	~Performance() = default;
	void Initialize();
	void ShowWidget();
	void GetWorkload();
private:

	void GetCPUTotalWorkload();
	void GetCPUEngineWorkload();
	void GetCPUTemperature();
	void GetMemorySizeInformation();
	void GetMemoryTotalUsage();
	void GetMemoryEngineUsage();

	static constexpr float updatePeriod = 0.5f;

	// Timer to avoid performace retrieval on every frame
	Timer timer;
	float lastSampleTime;

	// Variables to check memory
	MEMORYSTATUSEX memInfo;
	PROCESS_MEMORY_COUNTERS_EX pmc;

	std::unique_ptr<CPU> m_CPU;
	std::vector<std::unique_ptr<Disk>> m_Disks;
	std::vector<std::unique_ptr<GPU>> m_GPUs;
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