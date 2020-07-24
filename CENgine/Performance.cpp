#include "Performance.h"
#include "imgui/imgui.h"

Performance::Performance(Timer& timer)
	:
	timer(timer),
	m_CPUTotalWorkload(0.0f),
	m_CPUEngineWorkload(0.0f),
	m_CPUTemperature(0),
	m_TotalPhysicalMemory(0),
	m_TotalVirtualMemory(0),
	m_PhysicalMemoryTotalWorkload(0),
	m_PhysicalMemoryEngineWorkload(0),
	m_VirtualMemoryTotalWorkload(0),
	m_VirtualMemoryEngineWorkload(0),
	canReadCpu(true),					// Initialize the flag indicating whether this object can read the system CPU usage or not
	queryHandle(nullptr),
	counterHandle(nullptr),
	lastSampleTime(0.0f)
{
	m_GPU = Peripheral::GetPeripheral<GPU>();
}

void Performance::Initialize()
{
	// Retrieve memory information
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GetMemorySizeInformation();

	PDH_STATUS status;

	// Create a query object to poll CPU usage
	status = PdhOpenQuery(NULL, 0, &queryHandle);
	if(status != ERROR_SUCCESS)
	{
		canReadCpu = false;
	}

	// Set query object to poll all CPUs in the system
	status = PdhAddEnglishCounter(queryHandle, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &counterHandle);
	if(status != ERROR_SUCCESS)
	{
		canReadCpu = false;
	}

	status = PdhCollectQueryData(queryHandle);
	if(status != ERROR_SUCCESS)
	{
		canReadCpu = false;
	}

	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numberOfProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

	lastSampleTime = timer.Peek();

	m_GPU->Initialize();
}

void Performance::Shutdown()
{
	if(canReadCpu)
	{
		PdhCloseQuery(queryHandle);
	}
}

void Performance::ShowWidget()
{
	// THIS MUST GET OUT HERE!
	Calculate();

	if(ImGui::Begin("Performance"))
	{
		//ImGui::SetWindowFontScale(4.0f);
		ImGui::Separator();
		ImGui::Text("CPU Total Workload: %.0f%%", m_CPUTotalWorkload);
		ImGui::Text("CPU Engine Workload: %.0f%%", m_CPUEngineWorkload);
		ImGui::Text("CPU Temperature: %.0f°C", m_CPUTemperature);
		ImGui::Separator();
		ImGui::Text("System Memory Load: %d%%", m_MemoryLoad);
		ImGui::Text("Physical Memory Total Workload: %d/%d MB", m_PhysicalMemoryTotalWorkload, m_TotalPhysicalMemory);
		ImGui::Text("Physical Memory Engine Workload: %d/%d MB", m_PhysicalMemoryEngineWorkload, m_TotalPhysicalMemory);
		ImGui::Text("VM Total Workload: %d/%d MB", m_VirtualMemoryTotalWorkload, m_TotalVirtualMemory);
		ImGui::Text("VMEngine Workload: %d/%d MB", m_VirtualMemoryEngineWorkload, m_TotalVirtualMemory);
		ImGui::Separator();
		ImGui::Text("GPU Workload: %d%%", m_GPU->GetWorkload());
	}

	ImGui::End();
}

void Performance::Calculate()
{
	const auto now = timer.Peek();

	if(now > (lastSampleTime + updatePeriod))
	{
		lastSampleTime = now;
		GetCPUTotalWorkload();
		GetCPUEngineWorkload();
		GetCPUTemperature();
		GetMemoryTotalUsage();
		GetMemoryEngineUsage();
	}
}

void Performance::GetCPUTotalWorkload()
{
	PDH_FMT_COUNTERVALUE v;

	if(canReadCpu)
	{
		PDH_STATUS status;
		status = PdhCollectQueryData(queryHandle);

		if(status == ERROR_SUCCESS)
		{
			status = PdhGetFormattedCounterValue(counterHandle, PDH_FMT_DOUBLE, NULL, &v);
			if(status == ERROR_SUCCESS)
			{
				m_CPUTotalWorkload = v.doubleValue;
			}
		}
	}
}

void Performance::GetCPUEngineWorkload()
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numberOfProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	m_CPUEngineWorkload = percent * 100;
}

void Performance::GetCPUTemperature()
{
	//TODO: IMPLEMENT TEMPERATURE
}

void Performance::GetMemorySizeInformation()
{
	GlobalMemoryStatusEx(&memInfo);
	m_TotalPhysicalMemory = static_cast<unsigned long>(memInfo.ullTotalPhys / 1024 / 1024);								// Formats the value to MB
	m_TotalVirtualMemory = static_cast<unsigned long>(memInfo.ullTotalPageFile / 1024 / 1024);							// Formats the value to MB
}

void Performance::GetMemoryTotalUsage()
{
	GlobalMemoryStatusEx(&memInfo);
	m_MemoryLoad = (memInfo.dwMemoryLoad);
	m_PhysicalMemoryTotalWorkload = (memInfo.ullTotalPhys / 1024 / 1024) - (memInfo.ullAvailPhys / 1024 / 1024);		// Formats the value to MB
	m_VirtualMemoryTotalWorkload = (memInfo.ullTotalPageFile / 1024 / 1024) - (memInfo.ullAvailPageFile / 1024 / 1024);	// Formats the value to MB
}

void Performance::GetMemoryEngineUsage()
{
	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));
	m_PhysicalMemoryEngineWorkload = (pmc.WorkingSetSize / 1024 / 1024);												// Formats the value to MB
	m_VirtualMemoryEngineWorkload = (pmc.PrivateUsage / 1024 / 1024);													// Formats the value to MB
}
