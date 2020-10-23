#include "CPU.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

#include <intrin.h>

CPU::CPU(std::vector<std::unique_ptr<Core>>&& cores)
	:
	canReadCPU(true),
	m_Cores(std::move(cores)),
	m_CPUTotalWorkload(0.0f),
	m_CPUEngineWorkload(0.0f),
	queryHandle(nullptr),
	counterHandle(nullptr),
	self(nullptr),
	lastCPU({ 0 }),
	lastSysCPU({ 0 }),
	lastUserCPU({ 0 }),
	numProcessors(0),
	numberOfProcessors(0)
{

}

bool CPU::Initialize()
{
	PDH_STATUS status;

	// Create a query object to poll CPU usage
	status = PdhOpenQuery(NULL, 0, &queryHandle);
	if (status != ERROR_SUCCESS)
	{
		canReadCPU = false;
	}

	// Set query object to poll all CPUs in the system
	status = PdhAddEnglishCounter(queryHandle, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &counterHandle);
	if (status != ERROR_SUCCESS)
	{
		canReadCPU = false;
	}

	status = PdhCollectQueryData(queryHandle);
	if (status != ERROR_SUCCESS)
	{
		canReadCPU = false;
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

	int CPUInfo[4] = { -1 };
	char CPUBrandString[0x40];
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	memset(CPUBrandString, 0, sizeof(CPUBrandString));

	// Get the information associated with each extended ID.
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string.
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}

	// Force the string to end with \0 to avoid compile warnings
	CPUBrandString[0x39] = '\0';
	m_Name = CPUBrandString;

	bool valid = true;
	for (auto it = begin(m_Cores); it != end(m_Cores); it++)
	{
		int index = distance(m_Cores.begin(), it);
		if (!m_Cores[index]->Initialize())
		{
			m_Cores.erase(it--);
			valid = false;
		}
	}

	return valid;
}

CPU::~CPU()
{
	if (canReadCPU)
	{
		PdhCloseQuery(queryHandle);
	}
}

void CPU::ShowWidget()
{
	ImGui::Text("%s", m_Name.c_str());
	ImGui::Text("Total Usage: %.02f%%", m_CPUTotalWorkload);
	ImGui::Text("CENgine Usage: %.02f%%", m_CPUEngineWorkload);

	if (m_Cores.size() > 0)
	{
		ImGui::Text("        Idle	Kernel	Proc");
		for (const auto& core : m_Cores)
		{
			core->ShowWidget();
		}
	}
}

void CPU::GetWorkload()
{
	PDH_FMT_COUNTERVALUE v;
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	unsigned long long percent;

	// Get Total Workload
	if (canReadCPU)
	{
		PDH_STATUS status;
		status = PdhCollectQueryData(queryHandle);

		if (status == ERROR_SUCCESS)
		{
			status = PdhGetFormattedCounterValue(counterHandle, PDH_FMT_DOUBLE, NULL, &v);
			if (status == ERROR_SUCCESS)
			{
				m_CPUTotalWorkload = static_cast<float>(v.doubleValue);
			}
		}
	}

	// Get application workload
	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numberOfProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	m_CPUEngineWorkload = static_cast<float>(percent) * 100.0f;

	for (const auto& core : m_Cores)
	{
		core->GetWorkload();
	}
}