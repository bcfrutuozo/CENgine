#include "Disk.h"
#include "imgui/imgui.h"
#include "Utilities.h"
#include "MBRPartitionTable.h"
#include "GPTPartitionTable.h"
#include "MBRPartition.h"
#include "GPTPartition.h"

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

	CloseHandle(handle);

	switch (layout->PartitionStyle)
	{
	case PARTITION_STYLE_MBR:
		m_PartitionTable = std::unique_ptr<PartitionTable>(new MBRPartitionTable(layout->PartitionCount, layout->Mbr.CheckSum, layout->Mbr.Signature));

		for (int i = 0; i < layout->PartitionCount; ++i)
		{
			if (layout->PartitionEntry[i].Mbr.PartitionType != PARTITION_ENTRY_UNUSED)
			{
				m_PartitionTable->AddPartition(new MBRPartition(layout->PartitionEntry[i]));
			}
		}
		break;
	case PARTITION_STYLE_GPT:
		m_PartitionTable = std::unique_ptr<PartitionTable>(new GPTPartitionTable(layout->PartitionCount, GuidToString(layout->Gpt.DiskId), layout->Gpt.StartingUsableOffset.QuadPart, layout->Gpt.UsableLength.QuadPart));
		for (int i = 0; i < layout->PartitionCount; ++i)
		{
			auto p = new GPTPartition(layout->PartitionEntry[i]);

			if (p->IsLDMPartition())
			{
				m_HasLDMPartition = true;
			}

			m_PartitionTable->AddPartition(p);
		}
		break;
	}

	m_HeaderTitle = StringFormat("Disk %d - %s", m_Device.Index, m_Name.c_str());

	if (!m_HasLDMPartition)
	{
		m_LogicalName = std::string(GetLogicalName(m_Device.Index));
	}
}

void Disk::ShowWidget()
{
	if (ImGui::CollapsingHeader(m_HeaderTitle.c_str()))
	{
		if (!m_HasLDMPartition)
		{
			ImGui::Text("Logical name: %s", m_LogicalName.c_str());
			ImGui::Text("Free Space: %I64u MBs", m_FreeBytes);
		}
		else
		{
			ImGui::Text("Logical name: Dynamic Volume");
		}

		ImGui::Text("Cylinders: %d", m_Cylinders);
		ImGui::Text("Tracks per Cylinder = %d", m_TracksPerCylinder);
		ImGui::Text("Sectors per Track = %d", m_SectorsPerTrack);
		ImGui::Text("Bytes per Sector = %d", m_BytesPerSector);
		ImGui::Text("Disk Size: %d GBs", m_TotalSize);
		m_PartitionTable->ShowWidget();
	}
}

void Disk::GetWorkload()
{
	BOOL fResult;

	//GetDiskFreeSpaceEx function, which can get the space state of the drive disk, returns a BOOL-type data  

	ULARGE_INTEGER fbc = { 0 };
	ULARGE_INTEGER tb = { 0 };
	ULARGE_INTEGER fb = { 0 };

	fResult = GetDiskFreeSpaceEx(m_LogicalName.c_str(),
		(PULARGE_INTEGER)&fbc,
		(PULARGE_INTEGER)&tb,
		(PULARGE_INTEGER)&fb);

	m_FreeBytesToCaller = fbc.QuadPart;
	m_TotalBytes = tb.QuadPart;
	m_FreeBytes = fb.QuadPart;

	if (fResult)
	{
		m_TotalBytes = m_TotalBytes / 1048576;
		m_FreeBytes = m_FreeBytes / 1048576;
	}
}

bool Disk::CheckDriveIndex(unsigned int p_Index)
{
	if ((p_Index < DRIVE_INDEX_MIN) || (p_Index > DRIVE_INDEX_MAX))
	{
		return false;
	}

	return true;
}

const std::string Disk::GetPhysicalName(unsigned long DriveIndex)
{
	if (CheckDriveIndex(DriveIndex))
	{
		return StringFormat("\\\\.\\PHYSICALDRIVE%d", DriveIndex);
	}

	throw std::runtime_error("The specified drive index is not a valid one");
}

HANDLE Disk::GetPhysicalHandle(const std::string& physicalName)
{
	HANDLE hPhysical = INVALID_HANDLE_VALUE;
	hPhysical = GetHandle(physicalName.c_str());
	return hPhysical;
}

HANDLE Disk::GetLogicalHandle(const std::string& physicalName)
{
	HANDLE hLogical = INVALID_HANDLE_VALUE;
	hLogical = GetHandle(physicalName);
	return hLogical;
}

const std::string Disk::GetLogicalName(unsigned long DriveIndex)
{
	char volume_name[MAX_PATH];
	HANDLE hDrive = INVALID_HANDLE_VALUE, hVolume = INVALID_HANDLE_VALUE;
	size_t len;
	char path[MAX_PATH];
	VOLUME_DISK_EXTENTS DiskExtents;
	DWORD size;
	UINT drive_type;
	static const char* ignore_device[] = { "\\Device\\CdRom", "\\Device\\Floppy" };

	CheckDriveIndex(DriveIndex);

	for (int i = 0; ; ++i)
	{
		if (i == 0)
		{
			hVolume = FindFirstVolumeA(volume_name, sizeof(volume_name));
			if (hVolume == INVALID_HANDLE_VALUE)
			{
				printf_s("Could not access first GUID volume: %s\n", GetLastError());
			}
		}

		else
		{
			if (!FindNextVolumeA(hVolume, volume_name, sizeof(volume_name)))
			{
				if (GetLastError() != ERROR_NO_MORE_FILES)
				{
					printf_s("Could not access next GUID volume: %s\n", GetLastError());
				}
			}
		}

		// Sanity checks
		len = strlen(volume_name);
		if ((len <= 1) || (_strnicmp(volume_name, "\\\\?\\", 4) != 0) || (volume_name[len - 1] != '\\'))
		{
			continue;
		}

		drive_type = GetDriveTypeA(volume_name);
		volume_name[len - 1] = 0;

		if (QueryDosDeviceA(&volume_name[4], path, sizeof(path)) == 0)
		{
			continue;
		}

		int j;
		for (j = 0; (j < ARRAYSIZE(ignore_device)) && (_strnicmp(path, ignore_device[j], strlen(ignore_device[j])) != 0); j++);
		if (j < ARRAYSIZE(ignore_device))
		{
			continue;
		}

		// If we can't have FILE_SHARE_WRITE, forget it
		hDrive = CreateFileA(volume_name, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if (hDrive == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		if ((!DeviceIoControl(hDrive, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &DiskExtents, sizeof(DiskExtents), &size, NULL)) || (size <= 0))
		{
			CloseHandle(hDrive);
			continue;
		}

		CloseHandle(hDrive);
		if ((DiskExtents.NumberOfDiskExtents >= 1) && (DiskExtents.Extents[0].DiskNumber == DriveIndex))
		{
			// Adding backlash
			volume_name[len - 1] = '\\';
			break;
		}
	}

	if (hVolume != INVALID_HANDLE_VALUE)
		FindVolumeClose(hVolume);

	return std::string(volume_name);
}

HANDLE Disk::GetHandle(const std::string& path)
{
	HANDLE hDrive = INVALID_HANDLE_VALUE;

	if (path != "")
	{
		hDrive = CreateFileA(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

		if (hDrive == INVALID_HANDLE_VALUE)
		{
			//printf_s("Could not open drive %s: %s\n", Path, GetLastError());
		}
	}

	return hDrive;
}