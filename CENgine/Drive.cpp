#include "Drive.h"
#include "DDK.h"	// Driver development kit headers to get disk information
#include "Utilities.h"
#include "MBRPartitionTable.h"
#include "GPTPartitionTable.h"
#include "MBRPartition.h"
#include "GPTPartition.h"
#include "PeripheralThrowMacros.h"
#include "Volume.h"

#pragma warning(push, 0)
#include "imgui/imgui.h"
#pragma warning(pop)

#include <winioctl.h>

Drive::Drive(Device device)
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
	m_Type(Type::UNSUPPORTED),
	m_IsTrimEnabled(false),
	m_HasSeekPenalty(false),
	m_RPM(0u),
	Peripheral(device)
{
	if (m_Device.Service == L"disk")
	{
		m_Type = CheckIfDriveIsRemovable(ToNarrow(m_Device.Enumerator)) ? Type::REMOVABLE : Type::DISK;
		m_LogicalName = std::string(GetLogicalName(device.Index));
	}
	else if (m_Device.Service == L"cdrom")
	{
		m_Type = Type::CDROM;

		// Windows treats CDROM as 1-1 drive to volume. Which means the logical name of a CDROM is retrieving by looking the volumes
		m_LogicalName = Volume::FindVolumeByDriverEnumerator(ToNarrow(m_Device.Enumerator), Type::CDROM);
	}
	else
	{
		// Passes 0 as default UNSUPPORTED enum
		throw PRPH_UNSUPPORTED_TYPE_EXCEPT(0);
	}

	m_Name = GetPhysicalName(device.Index, m_Type);
}

Drive::~Drive()
{

}

bool Drive::Initialize()
{
	bool bResult;	// results flag
	DWORD junk;		// function return
	HANDLE handle = GetPhysicalHandle(m_Name);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// Get disk geometry information to calculate its size
	DISK_GEOMETRY geometry = { 0 };
	bResult = DeviceIoControl(handle,  // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
		NULL,
		0, // no input buffer
		&geometry, sizeof(DISK_GEOMETRY),     // output buffer
		&junk,                 // # bytes returned
		nullptr);  // synchronous I/O

	m_Cylinders = static_cast<unsigned long long>(geometry.Cylinders.QuadPart);
	m_TracksPerCylinder = geometry.TracksPerCylinder;
	m_SectorsPerTrack = geometry.SectorsPerTrack;
	m_BytesPerSector = geometry.BytesPerSector;
	m_TotalSize = ((m_Cylinders * m_TracksPerCylinder * m_SectorsPerTrack * m_BytesPerSector) / static_cast<unsigned long long>(1073741824)); // Showing the value as GBs

	// Get fixed media properties like Trim Enabled, Seek Penalty and RPM for HDDs.
	if (geometry.MediaType == FixedMedia)
	{
		// Check for TRIM to check for SSD type
		STORAGE_PROPERTY_QUERY spqTrim;
		spqTrim.PropertyId = (STORAGE_PROPERTY_ID)StorageDeviceTrimProperty;
		spqTrim.QueryType = PropertyStandardQuery;
		DEVICE_TRIM_DESCRIPTOR dtd = { 0 };
		junk = 0;
		bResult = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &spqTrim, sizeof(spqTrim), &dtd, sizeof(dtd), &junk, NULL);

		// Check for SEEKPENALTY to check for SSD type
		STORAGE_PROPERTY_QUERY spqSeekP;
		spqSeekP.PropertyId = (STORAGE_PROPERTY_ID)StorageDeviceSeekPenaltyProperty;
		spqSeekP.QueryType = PropertyStandardQuery;
		DEVICE_SEEK_PENALTY_DESCRIPTOR dspd = { 0 };
		junk = 0;
		bResult = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &spqSeekP, sizeof(spqSeekP), &dspd, sizeof(dspd), &junk, NULL);

		// Get device query to get device information like RPM
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
		m_IsTrimEnabled = dtd.TrimEnabled;
		m_HasSeekPenalty = dspd.IncursSeekPenalty;
		m_RPM = static_cast<unsigned int>(id_query.data[kNominalMediaRotRateWordIndex]);
	}

	STORAGE_DESCRIPTOR_HEADER sdh;
	STORAGE_PROPERTY_QUERY spq;
	spq.PropertyId = StorageDeviceProperty;
	spq.QueryType = PropertyStandardQuery;

	junk = 0;
	bResult = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &spq, sizeof(STORAGE_PROPERTY_QUERY), &sdh, sizeof(STORAGE_DESCRIPTOR_HEADER), &junk, NULL);

	BYTE buf[1024];
	junk = 0;
	bResult = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &spq, sizeof(STORAGE_PROPERTY_QUERY), &buf, 1024, &junk, NULL);

	PSTORAGE_DEVICE_DESCRIPTOR pDeviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)buf;
	m_ProductId = std::string(reinterpret_cast<const char*>(buf + pDeviceDescriptor->ProductIdOffset));
	m_SerialNumber = std::string(reinterpret_cast<const char*>(buf + pDeviceDescriptor->SerialNumberOffset));

	// Remove empty spaces from serial number
	m_SerialNumber.erase(std::remove_if(m_SerialNumber.begin(), m_SerialNumber.end(), std::isspace), m_SerialNumber.end());

	switch (m_Type)
	{
	case Type::CDROM:
		m_HeaderTitle = StringFormat("CD-ROM %s - %s", m_ProductId.c_str(), m_Name.c_str());
		break;
	case Type::REMOVABLE:
		m_HeaderTitle = StringFormat("Removable %s - %s", m_ProductId.c_str(), m_Name.c_str());
		break;
	case Type::DISK:
		m_HeaderTitle = StringFormat("Disk %s - %s", m_ProductId.c_str(), m_Name.c_str());
		break;
	}

	// Get layout information to get partition data
	BYTE buffer[4096];
	junk = 0;
	PDRIVE_LAYOUT_INFORMATION_EX layout = (PDRIVE_LAYOUT_INFORMATION_EX)buffer;
	bResult = DeviceIoControl(handle,  // device to be queried
		IOCTL_DISK_GET_DRIVE_LAYOUT_EX,  // operation to perform
		NULL,
		0, // no input buffer
		layout, // output buffer
		4096, // Assuming the system is GPT and each driver can handle up to 4 partitions.
		&junk,                 // # bytes returned
		nullptr);  // synchronous I/O

	CloseHandle(handle);

	if (m_Type != Type::CDROM)
	{
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

	return true;
}

void Drive::ShowWidget()
{
	if (ImGui::TreeNode(m_HeaderTitle.c_str()))
	{
		ImGui::Text("Serial number: %s", m_SerialNumber.c_str());
		ImGui::Text("Logical name: %s", m_LogicalName.c_str());

		bool isSSD = (m_IsTrimEnabled && !m_HasSeekPenalty && m_RPM == 1);

		switch (m_Type)
		{
		case Type::CDROM:
			ImGui::Text("Free Space: %I64u MBs", m_FreeBytes);
			ImGui::Text("Total Disk Size: %I64u MBs", m_TotalBytes);
			ImGui::Text("Type: CD-ROM");
			break;
		case Type::REMOVABLE:
			ImGui::Text("Free Space: %I64u MBs", m_FreeBytes);
			ImGui::Text("Total Disk Size: %I64u MBs", m_TotalBytes);
			ImGui::Text("Type: Removable");
			m_PartitionTable->ShowWidget();
			break;
		case Type::DISK:
			ImGui::Text("Free Space: %I64u GBs", m_FreeBytes);
			ImGui::Text("Total Disk Size: %I64u GBs", m_TotalBytes);
			ImGui::Text("Type: %s", isSSD ? "SSD" : "HDD");
			if (!isSSD)
			{
				ImGui::Text("Rotation: %u RPM", m_RPM);
				ImGui::Text("Cylinders: %d", m_Cylinders);
				ImGui::Text("Tracks per Cylinder = %d", m_TracksPerCylinder);
				ImGui::Text("Sectors per Track = %d", m_SectorsPerTrack);
				ImGui::Text("Bytes per Sector = %d", m_BytesPerSector);
			}
			m_PartitionTable->ShowWidget();
			break;
		}

		ImGui::TreePop();
	}
}

void Drive::GetWorkload()
{
	bool fResult;

	ULARGE_INTEGER fbc = { 0 };
	ULARGE_INTEGER tb = { 0 };
	ULARGE_INTEGER fb = { 0 };

	// TODO: CHECK FOR RAID 0 TO GET THE REAL AVAILABLE SPACE OF THE DISK.

	fResult = GetDiskFreeSpaceEx(m_LogicalName.c_str(),
		(PULARGE_INTEGER)&fbc,
		(PULARGE_INTEGER)&tb,
		(PULARGE_INTEGER)&fb);

	m_FreeBytesToCaller = fbc.QuadPart;
	m_TotalBytes = tb.QuadPart;
	m_FreeBytes = fb.QuadPart;

	if (fResult)
	{
		switch (m_Type)
		{
		case Type::CDROM:
		case Type::REMOVABLE:
			m_TotalBytes = m_TotalBytes / 1048576;
			m_FreeBytes = m_FreeBytes / 1048576;
			break;
		default:
			m_TotalBytes = m_TotalBytes / 1073741824;
			m_FreeBytes = m_FreeBytes / 1073741824;
			break;
		}
	}
}

const HANDLE Drive::GetHandle(std::string p_Path)
{
	HANDLE hDrive = INVALID_HANDLE_VALUE;

	// Remove last trailing backslash
	if (p_Path.back() == '\\')
	{
		p_Path.pop_back();
	}

	if (!p_Path.empty())
	{
		hDrive = CreateFileA(p_Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}

	return hDrive;
}

const HANDLE Drive::GetPhysicalHandle(const std::string& p_PhysicalName)
{
	HANDLE hPhysical = INVALID_HANDLE_VALUE;
	hPhysical = GetHandle(p_PhysicalName.c_str());
	return hPhysical;
}

const HANDLE Drive::GetLogicalHandle(const std::string& p_PhysicalName)
{
	HANDLE hLogical = INVALID_HANDLE_VALUE;
	hLogical = GetHandle(p_PhysicalName);
	return hLogical;
}

const std::string Drive::GetPhysicalName(const unsigned long p_DriveIndex, Type p_Type)
{

	switch (p_Type)
	{

	case Type::CDROM:
		return StringFormat("\\\\.\\CDROM%d", p_DriveIndex);

	case Type::REMOVABLE:
	case Type::DISK:

		if (CheckDriveIndex(p_DriveIndex))
		{
			return StringFormat("\\\\.\\PHYSICALDRIVE%d", p_DriveIndex);
		}
		throw PRPH_INVALID_INDEX_EXCEPT(p_DriveIndex);

	default:
		throw PRPH_UNSUPPORTED_TYPE_EXCEPT(static_cast<long>(p_Type));
		break;
	}
}

const std::string Drive::GetLogicalName(const unsigned long p_DriveIndex)
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
					if (GetLastError() == ERROR_NO_MORE_FILES)
					{
						return std::string("NULL");
					}
					else
					{
						throw PRPH_LAST_EXCEPT();
					}
				}
			}

			if (ValidateLogicalNameWithIndex(volume_name, p_DriveIndex))
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

const bool Drive::CheckDriveIndex(const unsigned int p_DriveIndex)
{
	if ((p_DriveIndex < DRIVE_INDEX_MIN) || (p_DriveIndex > DRIVE_INDEX_MAX))
	{
		return false;
	}

	return true;
}

const bool Drive::ValidateLogicalNameWithIndex(const char* p_VolumeName, const unsigned long p_DriveIndex)
{
	DWORD size;
	BYTE buffer[8192];	// To support until 256 DISK_EXTENT (24 bytes) structures (6144 bytes at total)
	PVOLUME_DISK_EXTENTS DiskExtents = (PVOLUME_DISK_EXTENTS)buffer;
	HANDLE hDrive = INVALID_HANDLE_VALUE;
	bool found = false;

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

bool Drive::CheckIfDriveIsRemovable(const std::string& p_DeviceService)
{
	if (StringContains(p_DeviceService, "SCSI"))
	{
		return false;
	}
	else if (StringContains(p_DeviceService, "USBSTOR\\Disk"))
	{
		return true;
	}
}
