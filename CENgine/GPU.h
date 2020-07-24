#pragma once

#if defined(_M_X64) || defined(__amd64__)
#define NVAPI_DLL "nvapi64.dll"
#else
#define NVAPI_DLL "nvapi.dll"
#endif

#include "Peripheral.h"

#include <memory>

class GPU : public Peripheral
{
public:

	GPU(Device driver);
	~GPU() = default;
};

class NvidiaGPU : public GPU
{
public:

	NvidiaGPU(Device driver);
	~NvidiaGPU() = default;
	void Initialize() override;
	void Shutdown() override;
	const long GetWorkload() override;
private:

	// Defined numbers and function pointers for GPU
	#define NVAPI_MAX_PHYSICAL_GPUS   64
	#define NVAPI_MAX_USAGES_PER_GPU  34
	typedef int* (*NvAPI_QueryInterface_t)(unsigned int offset);
	typedef int (*NvAPI_Initialize_t)();
	typedef int (*NvAPI_EnumPhysicalGPUs_t)(int** handles, int* count);
	typedef int (*NvAPI_GPU_GetUsages_t)(int* handle, unsigned int* usages);
	// nvapi.dll/nvapi64.dll internal function pointers
	NvAPI_QueryInterface_t      API_QueryInterface = nullptr;
	NvAPI_Initialize_t          API_Initialize = nullptr;
	NvAPI_EnumPhysicalGPUs_t    API_EnumPhysicalGPUs = nullptr;
	NvAPI_GPU_GetUsages_t       API_GPU_GetUsages = nullptr;
};

class AmdGPU : public GPU
{
public:

	AmdGPU(Device driver);
	~AmdGPU() = default;
	void Initialize() override;
	void Shutdown() override;
	const long GetWorkload() override;
private:
};