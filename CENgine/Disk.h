#pragma once

#include "Peripheral.h"
#include "PartitionTable.h"

#include <memory>

static constexpr unsigned int DRIVE_INDEX_MIN = 0x00000000;
static constexpr unsigned int DRIVE_INDEX_MAX = 0x000000C0;
static constexpr int kNominalMediaRotRateWordIndex = 217;

class Disk : public Peripheral
{
public:

	Disk(Device driver);
	~Disk();
	void Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;

private:

	enum class Type
	{
		HDD,
		SSD,
	};

	const std::string GetLogicalName(unsigned long p_DriveIndex);
	static const bool CheckDriveIndex(unsigned int p_DriveIndex);
	static const std::string GetPhysicalName(unsigned long p_DriveIndex);
	static const HANDLE GetPhysicalHandle(const std::string& p_PhysicalName);
	static const HANDLE GetLogicalHandle(const std::string& p_PhysicalName);
	static const bool ValidateLogicalNameWithDriveIndex(const char* p_VolumeName, const unsigned long p_DriveIndex);
	static const HANDLE GetHandle(std::string p_Path);

	Type m_Type;
	std::unique_ptr<PartitionTable> m_PartitionTable;
	std::string m_HeaderTitle;
	std::string m_LogicalName;
	unsigned int m_RPM;
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