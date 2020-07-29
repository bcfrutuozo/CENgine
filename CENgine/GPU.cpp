#include "GPU.h"
#include "NVIDIA_Functions.h"

GPU::GPU(Device device)
{
	m_Device = device;
}

NvidiaGPU::NvidiaGPU(Device device)
	:
	GPU(device)
{ }

void NvidiaGPU::Initialize()
{
	HMODULE hmod = LoadLibraryA(NVAPI_DLL);
	if(hmod == NULL)
	{
		return;
	}

	// nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
	API_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(hmod, "nvapi_QueryInterface");

	// Some useful internal functions that aren't exported by nvapi.dll
	API_Initialize = (NvAPI_Initialize_t)(*API_QueryInterface)(NvAPI_Initialize);
	API_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t)(*API_QueryInterface)(NvAPI_EnumPhysicalGPUs);
	API_GPU_GetUsages = (NvAPI_GPU_GetUsages_t)(*API_QueryInterface)(NvAPI_GPU_GetUsages);

	if(API_Initialize == nullptr || API_EnumPhysicalGPUs == nullptr || API_EnumPhysicalGPUs == nullptr || API_GPU_GetUsages == nullptr)
	{
		return;
	}

	// Initialize NvAPI library, call it once before calling any other NvAPI functions
	if((*API_Initialize)() != 0)
	{
		return;
	}
}

void NvidiaGPU::Shutdown()
{
	API_QueryInterface = nullptr;
	API_Initialize = nullptr;
	API_EnumPhysicalGPUs = nullptr;
	API_GPU_GetUsages = nullptr;
}

const long NvidiaGPU::GetWorkload()
{
	int gpuCount = 0;
	int* gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
	unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

	// gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
	gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

	(*API_EnumPhysicalGPUs)(gpuHandles, &gpuCount);
	(*API_GPU_GetUsages)(gpuHandles[0], gpuUsages);

	m_Workload = gpuUsages[3];

	return m_Workload;
}

AmdGPU::AmdGPU(Device device)
	:
	GPU(device)
{ }

void AmdGPU::Initialize()
{ }

void AmdGPU::Shutdown()
{

}

const long AmdGPU::GetWorkload()
{
	return 0;
}

IntelGPU::IntelGPU(Device device)
	:
	GPU(device)
{ }

void IntelGPU::Initialize()
{ }

void IntelGPU::Shutdown()
{

}

const long IntelGPU::GetWorkload()
{
	return 0;
}