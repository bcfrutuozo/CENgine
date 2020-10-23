#pragma once

#include "CENgineWin.h"
#include "Peripheral.h"
#include "PartitionTable.h"

#include <memory>

static constexpr unsigned int DRIVE_INDEX_MIN = 0x00000000;
static constexpr unsigned int DRIVE_INDEX_MAX = 0x000000C0;
static constexpr int kNominalMediaRotRateWordIndex = 217;

class Drive : public Peripheral
{
public:

	enum class Type
	{
		UNSUPPORTED,
		CDROM,
		REMOVABLE,
		DISK
	};

	Drive(Device driver);
	~Drive();
	bool Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;

private:

	static const HANDLE GetHandle(std::string p_Path);
	static const HANDLE GetPhysicalHandle(const std::string& p_PhysicalName);
	static const HANDLE GetLogicalHandle(const std::string& p_PhysicalName);
	static const std::string GetPhysicalName(const unsigned long p_DriveIndex, Type p_Type);
	static const std::string GetLogicalName(const unsigned long p_DriveIndex);
	static const bool CheckDriveIndex(const unsigned int p_DriveIndex);
	static const bool ValidateLogicalNameWithIndex(const char* p_VolumeName, const unsigned long p_DriveIndex);
	static bool CheckIfDriveIsRemovable(const std::string& p_DeviceService);


	Type m_Type;
	std::unique_ptr<PartitionTable> m_PartitionTable;
	std::string m_HeaderTitle;
	std::string m_LogicalName;
	std::string m_ProductId;
	std::string m_SerialNumber;
	bool m_IsTrimEnabled;
	bool m_HasSeekPenalty;
	unsigned int m_RPM;
	unsigned long long m_Cylinders;
	unsigned long long m_TracksPerCylinder;
	unsigned long long m_SectorsPerTrack;
	unsigned long long m_BytesPerSector;
	unsigned long long m_TotalSize;
	unsigned long long m_TotalBytes;
	unsigned long long m_FreeBytes;
	unsigned long long m_FreeBytesToCaller;
	unsigned int m_MaxPartitionCount;
};