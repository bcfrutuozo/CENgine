#include "Disk.h"
#include "DDK.h"	// Driver development kit headers to get disk information
#include "imgui/imgui.h"
#include "Utilities.h"
#include "MBRPartitionTable.h"
#include "GPTPartitionTable.h"
#include "MBRPartition.h"
#include "GPTPartition.h"
#include "PeripheralThrowMacros.h"

#include <winioctl.h>

Disk::Disk(Device device)
	:
	m_Cylinders(0),
	m_TracksPerCylinder(0),
	m_SectorsPerTrack(0),
	m_BytesPerSector(0),
	m_TotalSize(0),
	m_TotalBytes(0),
	m_FreeBytes(0),
	m_FreeBytesToCaller(0),
	m_MaxPartitionCount(0),
	Peripheral(device)
{
	m_Name = (GetPhysicalName(device.Index));
	m_HeaderTitle = StringFormat("Disk %d - %s", device.Index, m_Name.c_str());
	m_LogicalName = std::string(GetLogicalName(device.Index));
}

Disk::~Disk()
{

}

void Disk::Initialize()
{
	BOOL bResult;                 // results flag
	DWORD junk;                   // discard results

	DISK_GEOMETRY geometry = { 0 };
	HANDLE handle = GetPhysicalHandle(m_Name);

	bResult = DeviceIoControl(handle,  // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
		NULL,
		0, // no input buffer
		&geometry, sizeof(DISK_GEOMETRY),     // output buffer
		&junk,                 // # bytes returned
		nullptr);  // synchronous I/O

	m_Cylinders = geometry.Cylinders.QuadPart;
	m_TracksPerCylinder = geometry.TracksPerCylinder;
	m_SectorsPerTrack = geometry.SectorsPerTrack;
	m_BytesPerSector = geometry.BytesPerSector;
	m_TotalSize = ((m_Cylinders * m_TracksPerCylinder * m_SectorsPerTrack * m_BytesPerSector) / 1073741824); // Showing the value as GBs

	BYTE buffer[4096];
	PDRIVE_LAYOUT_INFORMATION_EX layout = (PDRIVE_LAYOUT_INFORMATION_EX)buffer;
	bResult = DeviceIoControl(handle,  // device to be queried
		IOCTL_DISK_GET_DRIVE_LAYOUT_EX,  // operation to perform
		NULL,
		0, // no input buffer
		layout, // output buffer
		4096, // Assuming the system is GPT and each driver can handle up to 4 partitions.
		&junk,                 // # bytes returned
		nullptr);  // synchronous I/O


	STORAGE_PROPERTY_QUERY spqTrim;
	spqTrim.PropertyId = (STORAGE_PROPERTY_ID)StorageDeviceTrimProperty;
	spqTrim.QueryType = PropertyStandardQuery;

	DEVICE_TRIM_DESCRIPTOR dtd = { 0 };
	junk = 0;
	bResult = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &spqTrim, sizeof(spqTrim), &dtd, sizeof(dtd), &junk, NULL);

	// Check for TRIM dtd.TrimEnabled

	STORAGE_PROPERTY_QUERY spqSeekP;
	spqSeekP.PropertyId = (STORAGE_PROPERTY_ID)StorageDeviceSeekPenaltyProperty;
	spqSeekP.QueryType = PropertyStandardQuery;

	// Check for SEEKPENALTY dspd.IncursSeekPenalty 

	junk = 0;
	DEVICE_SEEK_PENALTY_DESCRIPTOR dspd = { 0 };
	bResult = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &spqSeekP, sizeof(spqSeekP), &dspd, sizeof(dspd), &junk, NULL);

	ATAIdentifyDeviceQuery id_query;
	memset(&id_query, 0, sizeof(id_query));

	id_query.header.Length = sizeof(id_query.header);
	id_query.header.AtaFlags = ATA_FLAGS_DATA_IN;
	id_query.header.DataTransferLength = sizeof(id_query.data);
	id_query.header.TimeOutValue = 3;   //Timeout in seconds
	id_query.header.DataBufferOffset = offsetof(ATAIdentifyDeviceQuery, data[0]);
	id_query.header.CurrentTaskFile[6] = 0xec; // ATA IDENTIFY DEVICE

	junk = 0;
	bResult = DeviceIoControl(handle, IOCTL_ATA_PASS_THROUGH, &id_query, sizeof(id_query), &id_query, sizeof(id_query), &junk, NULL);

	//Index of nominal media rotation rate
	//SOURCE: http://www.t13.org/documents/UploadedDocuments/docs2009/d2015r1a-ATAATAPI_Command_Set_-_2_ACS-2.pdf
	//          7.18.7.81 Word 217
	//QUOTE: Word 217 indicates the nominal media rotation rate of the device and is defined in table:
	//          Value           Description
	//          --------------------------------
	//          0000h           Rate not reported
	//          0001h           Non-rotating media (e.g., solid state device)
	//          0002h-0400h     Reserved
	//          0401h-FFFEh     Nominal media rotation rate in rotations per minute (rpm)
	//                                  (e.g., 7 200 rpm = 1C20h)
	//          FFFFh           Reserved
	m_RPM = (UINT)id_query.data[kNominalMediaRotRateWordIndex];

	if (dtd.TrimEnabled == 1 && dspd.IncursSeekPenalty == 0 && m_RPM == 1)
	{
		m_Type = Type::SSD;
	}
	else
	{
		m_Type = Type::HDD;
	}

	CloseHandle(handle);

	switch (layout->PartitionStyle)
	{
	case PARTITION_STYLE_MBR:
		m_PartitionTable = std::unique_ptr<PartitionTable>(new MBRPartitionTable(layout->PartitionCount, layout->Mbr.CheckSum, layout->Mbr.Signature));

		for (unsigned long i = 0; i < layout->PartitionCount; ++i)
		{
			if (layout->PartitionEntry[i].Mbr.PartitionType != PARTITION_ENTRY_UNUSED)
			{
				m_PartitionTable->AddPartition(new MBRPartition(layout->PartitionEntry[i]));
			}
		}
		break;
	case PARTITION_STYLE_GPT:
		m_PartitionTable = std::unique_ptr<PartitionTable>(new GPTPartitionTable(layout->PartitionCount, GuidToString(layout->Gpt.DiskId), layout->Gpt.StartingUsableOffset.QuadPart, layout->Gpt.UsableLength.QuadPart));
		for (unsigned long i = 0; i < layout->PartitionCount; ++i)
		{
			m_PartitionTable->AddPartition(new GPTPartition(layout->PartitionEntry[i]));
		}
		break;
	}
}

void Disk::ShowWidget()
{
	if (ImGui::CollapsingHeader(m_HeaderTitle.c_str()))
	{
		ImGui::Text("Logical name: %s", m_LogicalName.c_str());
		ImGui::Text("Free Space: %I64u GBs", m_FreeBytes);
		ImGui::Text("Total Disk Size: %d GBs", m_TotalSize);
		ImGui::Text("Type: %s", m_Type == Type::HDD ? "HDD" : "SSD");
		if (m_Type == Type::HDD)
		{
			ImGui::Text("Rotation: %u RPM", m_RPM);
			ImGui::Text("Cylinders: %d", m_Cylinders);
			ImGui::Text("Tracks per Cylinder = %d", m_TracksPerCylinder);
			ImGui::Text("Sectors per Track = %d", m_SectorsPerTrack);
			ImGui::Text("Bytes per Sector = %d", m_BytesPerSector);
		}
		m_PartitionTable->ShowWidget();
	}
}

void Disk::GetWorkload()
{
	// No processing for Disk. VOLUMES refers to the letters and drives in Windows.
	BOOL fResult;

	ULARGE_INTEGER fbc = { 0 };
	ULARGE_INTEGER tb = { 0 };
	ULARGE_INTEGER fb = { 0 };

	// CHECK FOR RAID 0 TO GET THE REAL AVAILABLE SPACE OF THE DISK.

	fResult = GetDiskFreeSpaceEx(m_LogicalName.c_str(),
		(PULARGE_INTEGER)&fbc,
		(PULARGE_INTEGER)&tb,
		(PULARGE_INTEGER)&fb);

	m_FreeBytesToCaller = fbc.QuadPart;
	m_TotalBytes = tb.QuadPart;
	m_FreeBytes = fb.QuadPart;

	if (fResult)
	{
		m_TotalBytes = m_TotalBytes / 1073741824;
		m_FreeBytes = m_FreeBytes / 1073741824;
	}
}

const bool Disk::CheckDriveIndex(unsigned int p_DriveIndex)
{
	if ((p_DriveIndex < DRIVE_INDEX_MIN) || (p_DriveIndex > DRIVE_INDEX_MAX))
	{
		return false;
	}

	return true;
}

const std::string Disk::GetPhysicalName(unsigned long p_DriveIndex)
{
	if (CheckDriveIndex(p_DriveIndex))
	{
		return StringFormat("\\\\.\\PHYSICALDRIVE%d", p_DriveIndex);
	}

	throw std::runtime_error("The specified drive index is not a valid one");
}

const HANDLE Disk::GetPhysicalHandle(const std::string& p_PhysicalName)
{
	HANDLE hPhysical = INVALID_HANDLE_VALUE;
	hPhysical = GetHandle(p_PhysicalName.c_str());
	return hPhysical;
}

const HANDLE Disk::GetLogicalHandle(const std::string& p_PhysicalName)
{
	HANDLE hLogical = INVALID_HANDLE_VALUE;
	hLogical = GetHandle(p_PhysicalName);
	return hLogical;
}

const std::string Disk::GetLogicalName(unsigned long p_DriveIndex)
{
	char volume_name[50];
	HANDLE hVolume = INVALID_HANDLE_VALUE;

	if (CheckDriveIndex(p_DriveIndex))
	{
		for (int i = 0; ; ++i)
		{
			bool found = false;

			if (i == 0)
			{
				hVolume = FindFirstVolumeA(volume_name, sizeof(volume_name));
				if (hVolume == INVALID_HANDLE_VALUE)
				{
					throw PRPH_LAST_EXCEPT();
				}
			}
			else
			{
				if (FindNextVolumeA(hVolume, volume_name, sizeof(volume_name)) == 0)
				{
					throw PRPH_LAST_EXCEPT();
				}
			}

			if (ValidateLogicalNameWithDriveIndex(volume_name, p_DriveIndex))
			{
				FindVolumeClose(hVolume);
				return std::string(volume_name);
			}
		}
	}
	else
	{
		throw PRPH_INVALID_INDEX_EXCEPT(p_DriveIndex);
	}
}

const bool Disk::ValidateLogicalNameWithDriveIndex(const char* p_VolumeName, const unsigned long p_DriveIndex)
{
	DWORD size;
	BYTE buffer[8192];	// To support until 256 DISK_EXTENT (24 bytes) structures (6144 bytes at total)
	PVOLUME_DISK_EXTENTS DiskExtents = (PVOLUME_DISK_EXTENTS)buffer;
	HANDLE hDrive = INVALID_HANDLE_VALUE;
	bool found = false;

	if (GetDriveTypeA(p_VolumeName) != DRIVE_FIXED)
	{
		return false;
	}

	hDrive = GetHandle(p_VolumeName);

	if ((!DeviceIoControl(hDrive, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, DiskExtents, 8192, &size, NULL)) || (size <= 0))
	{
		CloseHandle(hDrive);
		return false;
	}
	
	if ((DiskExtents->NumberOfDiskExtents >= 1))
	{
		// Adding backlash
		for (unsigned int i = 0; i < DiskExtents->NumberOfDiskExtents; ++i)
		{
			if ((DiskExtents->Extents[i].DiskNumber == p_DriveIndex))
			{
				found = true;
				break;
			}
		}
	}

	CloseHandle(hDrive);

	return found;
}

const HANDLE Disk::GetHandle(std::string p_Path)
{
	HANDLE hDrive = INVALID_HANDLE_VALUE;

	// Remove last trailing backslash
	if (p_Path.back() == '\\')
	{
		p_Path.pop_back();
	}

	if (p_Path != "")
	{
		hDrive = CreateFileA(p_Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (hDrive == INVALID_HANDLE_VALUE)
		{
			throw PRPH_LAST_EXCEPT();
		}
	}

	return hDrive;
}