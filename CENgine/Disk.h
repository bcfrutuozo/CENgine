#pragma once

#include "Peripheral.h"
#include "PartitionTable.h"

#include <memory>

class Disk : public Peripheral
{
public:

	Disk(Device driver);
	~Disk();
	void Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
	static const char* GetLogicalName(unsigned long DriveIndex, bool bKeepTrailingBackslash);
private:

	static const std::string GetPhysicalName(unsigned long DriveIndex);
	static HANDLE GetPhysicalHandle(const std::string& physicalName);
	static HANDLE GetLogicalHandle(const std::string& physicalName);
	static HANDLE GetHandle(const std::string& physicalName);

	std::unique_ptr<PartitionTable> m_PartitionTable;
	long long m_Cylinders;
	unsigned long m_TracksPerCylinder;
	unsigned long m_SectorsPerTrack;
	unsigned long m_BytesPerSector;
	unsigned long m_TotalSize;
	unsigned long long m_TotalBytes;
	unsigned long long m_FreeBytes;
	unsigned long long m_FreeBytesToCaller;
	unsigned int m_MaxPartitionCount;
};