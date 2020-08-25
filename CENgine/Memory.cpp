#include "Memory.h"
#include "SMBIOS.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

Memory::Memory(const TypePhysicalMemory& p_PhysicalMemory, std::vector<std::unique_ptr<MemoryBank>>&& p_MemoryBanks)
	:
	m_Location(p_PhysicalMemory.Location),
	m_Use(p_PhysicalMemory.Use),
	m_ErrorCorrection(p_PhysicalMemory.ErrorCorrection),
	m_MaximumCapacity(p_PhysicalMemory.MaximumCapacity),
	m_ErrorInformationHandle(p_PhysicalMemory.ErrorInformationHandle),
	m_NumberDevices(p_PhysicalMemory.NumberDevices),
	m_ExtendedMaximumCapacity(p_PhysicalMemory.ExtendedMaximumCapacity),
	m_MemoryBanks(std::move(p_MemoryBanks))
{
	m_MemoryStatus.dwLength = sizeof(m_MemoryStatus);
}

Memory::~Memory()
{
}

void Memory::Initialize()
{
	GlobalMemoryStatusEx(&m_MemoryStatus);
	m_TotalPhysicalMemory = (m_MemoryStatus.ullTotalPhys / 1048576);	// Formats the value to MB
	m_TotalVirtualMemory = (m_MemoryStatus.ullTotalPageFile / 1048576);	// Formats the value to MB

	for (const auto& memory : m_MemoryBanks)
	{
		memory->Initialize();
	}
}

void Memory::ShowWidget()
{
	ImGui::Text("System Memory Load: %d%%", m_MemoryLoad);
	ImGui::Text("Physical Memory Total Workload: %d/%d MB", m_PhysicalMemoryTotalWorkload, m_TotalPhysicalMemory);
	ImGui::Text("Physical Memory Engine Workload: %d/%d MB", m_PhysicalMemoryEngineWorkload, m_TotalPhysicalMemory);
	ImGui::Text("VM Total Workload: %d/%d MB", m_VirtualMemoryTotalWorkload, m_TotalVirtualMemory);
	ImGui::Text("VM Engine Workload: %d/%d MB", m_VirtualMemoryEngineWorkload, m_TotalVirtualMemory);

	for (const auto& memory : m_MemoryBanks)
	{
		memory->ShowWidget();
	}
}

void Memory::GetWorkload()
{
	GlobalMemoryStatusEx(&m_MemoryStatus);
	m_MemoryLoad = m_MemoryStatus.dwMemoryLoad;
	m_PhysicalMemoryTotalWorkload = (m_MemoryStatus.ullTotalPhys / 1048576) - (m_MemoryStatus.ullAvailPhys / 1048576);		// Formats the value to MB
	m_VirtualMemoryTotalWorkload = (m_MemoryStatus.ullTotalPageFile / 1048576) - (m_MemoryStatus.ullAvailPageFile / 1048576);	// Formats the value to MB

	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&m_MemoryCounters), sizeof(m_MemoryCounters));
	m_PhysicalMemoryEngineWorkload = (m_MemoryCounters.WorkingSetSize / 1048576);												// Formats the value to MB
	m_VirtualMemoryEngineWorkload = (m_MemoryCounters.PrivateUsage / 1048576);													// Formats the value to MB

	for (const auto& memory : m_MemoryBanks)
	{
		memory->GetWorkload();
	}
}
