#pragma once

#include "GPU.h"
#include "nvml.h"
#include "utils.h"

class NvidiaGPU : public GPU
{
public:
	
	NvidiaGPU(Device device);
	~NvidiaGPU();
	bool Initialize() override;
	void GetWorkload() override;
	
private:

	//// Defined numbers and function pointers for GPU
	//#define NVAPI_MAX_PHYSICAL_GPUS   64
	//#define NVAPI_MAX_USAGES_PER_GPU  34
	//typedef int* (*NvAPI_QueryInterface_t)(unsigned int offset);
	//typedef int (*NvAPI_Initialize_t)();
	//typedef int (*NvAPI_EnumPhysicalGPUs_t)(int** handles, int* count);
	//typedef int (*NvAPI_GPU_GetUsages_t)(int* handle, unsigned int* usages);
	//// nvapi.dll/nvapi64.dll internal function pointers
	//NvAPI_QueryInterface_t      API_QueryInterface = nullptr;
	//NvAPI_Initialize_t          API_Initialize = nullptr;
	//NvAPI_EnumPhysicalGPUs_t    API_EnumPhysicalGPUs = nullptr;
	//NvAPI_GPU_GetUsages_t       API_GPU_GetUsages = nullptr;

	NVML libraryAccess;
	NVMLDeviceManager deviceManager;
	std::string  m_NVMLVersion;
	std::vector<NVMLDevice>::iterator i;
};