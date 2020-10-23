#pragma once

#include "CENgineWin.h"
#include "Peripheral.h"
#include "MemoryBank.h"

#include <memory>
#include <Psapi.h>

struct TypePhysicalMemory;

class Memory : public Peripheral
{
public:

	Memory(const TypePhysicalMemory& p_PhysicalMemory, std::vector<std::unique_ptr<MemoryBank>>&& p_MemoryBanks);
	~Memory();
	bool Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
private:

	MEMORYSTATUSEX m_MemoryStatus;
	PROCESS_MEMORY_COUNTERS_EX m_MemoryCounters;

	std::vector<std::unique_ptr<MemoryBank>> m_MemoryBanks;
	unsigned char m_Location;
	unsigned char m_Use;
	unsigned char m_ErrorCorrection;
	unsigned int m_MaximumCapacity;
	unsigned short m_ErrorInformationHandle;
	unsigned short m_NumberDevices;
	unsigned long long m_ExtendedMaximumCapacity;
	long m_MemoryLoad;
	unsigned long long m_TotalPhysicalMemory;
	unsigned long long m_TotalVirtualMemory;
	unsigned long long m_VirtualMemoryTotalWorkload;
	unsigned long long m_VirtualMemoryEngineWorkload;
	unsigned long long m_PhysicalMemoryTotalWorkload;
	unsigned long long m_PhysicalMemoryEngineWorkload;
};

