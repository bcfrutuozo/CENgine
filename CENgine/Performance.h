#pragma once

#include "Timer.h"

#include <iostream>
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
	void GetGPUWorkload();

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

	// Defined numbers and function pointers for GPU
	#define NVAPI_MAX_PHYSICAL_GPUS   64
	#define NVAPI_MAX_USAGES_PER_GPU  34
	typedef int* (*NvAPI_QueryInterface_t)(unsigned int offset);
	typedef int (*NvAPI_Initialize_t)();
	typedef int (*NvAPI_EnumPhysicalGPUs_t)(int** handles, int* count);
	typedef int (*NvAPI_GPU_GetUsages_t)(int* handle, unsigned int* usages);
	// nvapi64.dll internal function pointers
	NvAPI_QueryInterface_t      NvAPI_QueryInterface;
	NvAPI_Initialize_t          NvAPI_Initialize;
	NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs;
	NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages;


	float m_CPUTotalWorkload;
	float m_CPUEngineWorkload;
	long m_CPUTemperature;
	long m_MemoryLoad;
	int m_GPUWorkload;
	unsigned long m_TotalPhysicalMemory;
	unsigned long m_TotalVirtualMemory;
	unsigned long m_VirtualMemoryTotalWorkload;
	unsigned long m_VirtualMemoryEngineWorkload;
	unsigned long m_PhysicalMemoryTotalWorkload;
	unsigned long m_PhysicalMemoryEngineWorkload;
};