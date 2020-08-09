#pragma once

#include "Peripheral.h"
#include "PartitionTable.h"

#include <memory>

#define DRIVE_INDEX_MIN             0x00000000
#define DRIVE_INDEX_MAX             0x000000C0

static constexpr GUID LDMPartition = { 0x5808C8AA, 0x7E8F, 0x42E0, { 0x85, 0xD2, 0xE1, 0xE9, 0x04, 0x34, 0xCF, 0xB3} };

class Disk : public Peripheral
{
public:

	Disk(Device driver);
	~Disk();
	void Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
	const std::string GetLogicalName(unsigned long DriveIndex);
private:

	static bool CheckDriveIndex(unsigned int p_Index);
	static const std::string GetPhysicalName(unsigned long DriveIndex);
	static HANDLE GetPhysicalHandle(const std::string& physicalName);
	static HANDLE GetLogicalHandle(const std::string& physicalName);
	static HANDLE GetHandle(const std::string& physicalName);

	std::unique_ptr<PartitionTable> m_PartitionTable;
	std::string m_HeaderTitle;
	std::string m_LogicalName;
	bool m_HasLDMPartition;
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