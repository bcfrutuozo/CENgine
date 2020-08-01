#include "NvidiaGPU.h"
#include "NVIDIA_Functions.h"
#include "imgui/imgui.h"

NvidiaGPU::NvidiaGPU(Device device)
	:
	GPU(device),
	deviceManager(libraryAccess)
{ }

NvidiaGPU::~NvidiaGPU()
{ }

void NvidiaGPU::Initialize()
{
	const auto info = libraryAccess.get_info();

	// Get the number of Nvidia devices
	m_DriverInfo = info.driver_version;
	m_NVMLVersion = info.nvml_version;
	m_GPUCount = deviceManager.get_devices_count();

	i = deviceManager.devices_begin() + m_Device.Index;

	const auto& device = i->get_info();
	m_Name = device.name;
}

void NvidiaGPU::GetWorkload()
{
	i->refresh_metrics_or_halt();
	const auto& device = i->get_info();
	m_FanSpeed = device.metrics.fan_speed;
	m_Temperature = device.metrics.temperature;
	m_PowerUsage = device.metrics.power_usage;
	m_GPUUtilization = device.metrics.gpu_utilization;
	m_MemoryUtilization = device.metrics.memory_utilization;
}