#include "Performance.h"
#include "imgui/imgui.h"
#include "Utilities.h"

Performance::Performance(Timer& timer)
	:
	timer(timer),
	m_TotalPhysicalMemory(0),
	m_TotalVirtualMemory(0),
	m_PhysicalMemoryTotalWorkload(0),
	m_PhysicalMemoryEngineWorkload(0),
	m_VirtualMemoryTotalWorkload(0),
	m_VirtualMemoryEngineWorkload(0),
	m_MemoryLoad(0),
	memInfo({0}),
	pmc({0}),
	lastSampleTime(0.0f),
	m_CPU(Hardware::GetDevice<CPU>()),
	m_GPUs(Hardware::GetDevices<GPU>()),
	m_Disks(Hardware::GetDevices<Disk>()),
	m_Volumes(Hardware::GetDevices<Volume>())
{

}

void Performance::Initialize()
{
	// Retrieve memory information
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GetMemorySizeInformation();

	lastSampleTime = timer.Peek();

	m_CPU->Initialize();
	for(const auto& gpu : m_GPUs)
	{
		gpu->Initialize();
	}

	for(const auto& disk : m_Disks)
	{
		disk->Initialize();
	}

	for (const auto& volume : m_Volumes)
	{
		volume->Initialize();
	}
}

void Performance::ShowWidget()
{
	if(ImGui::Begin("Performance"))
	{
		ImGui::Separator();
		m_CPU->ShowWidget();
		
		ImGui::Separator();
		ImGui::Text("System Memory Load: %d%%", m_MemoryLoad);
		ImGui::Text("Physical Memory Total Workload: %d/%d MB", m_PhysicalMemoryTotalWorkload, m_TotalPhysicalMemory);
		ImGui::Text("Physical Memory Engine Workload: %d/%d MB", m_PhysicalMemoryEngineWorkload, m_TotalPhysicalMemory);
		ImGui::Text("VM Total Workload: %d/%d MB", m_VirtualMemoryTotalWorkload, m_TotalVirtualMemory);
		ImGui::Text("VMEngine Workload: %d/%d MB", m_VirtualMemoryEngineWorkload, m_TotalVirtualMemory);
		
		ImGui::Separator();
		for(const auto& gpu : m_GPUs)
		{
			gpu->ShowWidget();
		}
		
		ImGui::Separator();
		for(const auto& disk : m_Disks)
		{
			disk->ShowWidget();
		}

		ImGui::Separator();
		for (const auto& volume : m_Volumes)
		{
			volume->ShowWidget();
		}
	}

	ImGui::End();
}

void Performance::GetWorkload()
{
	const auto now = timer.Peek();

	if(now > (lastSampleTime + updatePeriod))
	{
		lastSampleTime = now;
		GetMemoryTotalUsage();
		GetMemoryEngineUsage();
		
		m_CPU->GetWorkload();
		
		for(const auto& gpu : m_GPUs)
		{
			gpu->GetWorkload();
		}

		for(const auto& disk : m_Disks)
		{
			disk->GetWorkload();
		}

		for (const auto& volume : m_Volumes)
		{
			volume->GetWorkload();
		}
	}
}

void Performance::GetCPUTotalWorkload()
{

}

void Performance::GetCPUEngineWorkload()
{

}

void Performance::GetCPUTemperature()
{
	//TODO: IMPLEMENT TEMPERATURE
}

void Performance::GetMemorySizeInformation()
{
	GlobalMemoryStatusEx(&memInfo);
	m_TotalPhysicalMemory = static_cast<unsigned long>(memInfo.ullTotalPhys / 1048576);								// Formats the value to MB
	m_TotalVirtualMemory = static_cast<unsigned long>(memInfo.ullTotalPageFile / 1048576);							// Formats the value to MB
}

void Performance::GetMemoryTotalUsage()
{
	GlobalMemoryStatusEx(&memInfo);
	m_MemoryLoad = (memInfo.dwMemoryLoad);
	m_PhysicalMemoryTotalWorkload = (memInfo.ullTotalPhys / 1048576) - (memInfo.ullAvailPhys / 1048576);		// Formats the value to MB
	m_VirtualMemoryTotalWorkload = (memInfo.ullTotalPageFile / 1048576) - (memInfo.ullAvailPageFile / 1048576);	// Formats the value to MB
}

void Performance::GetMemoryEngineUsage()
{
	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));
	m_PhysicalMemoryEngineWorkload = (pmc.WorkingSetSize / 1048576);												// Formats the value to MB
	m_VirtualMemoryEngineWorkload = (pmc.PrivateUsage / 1048576);													// Formats the value to MB
}