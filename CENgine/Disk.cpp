#include "Disk.h"
#include "imgui/imgui.h"
#include "Utilities.h"
#include "MBRPartitionTable.h"
#include "GPTPartitionTable.h"
#include "MBRPartition.h"
#include "GPTPartition.h"

#include <winioctl.h>
#pragma pack(pop)

#define DRIVE_INDEX_MIN             0x00000000
#define DRIVE_INDEX_MAX             0x000000C0

#define CheckDriveIndex(DriveIndex) \
	if ((DriveIndex < DRIVE_INDEX_MIN) || (DriveIndex > DRIVE_INDEX_MAX)) { \
		OutputDebugString("WARNING: Bad index value. Please check the code!\n"); \
	} \
	DriveIndex -= DRIVE_INDEX_MIN;

#define UEFI_SIGNATURE             0x49464555	// 'U', 'E', 'F', 'I', as a 32 bit little endian longword

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
		m_PartitionTable = std::unique_ptr<PartitionTable>(new MBRPartitionTable(layout->PartitionCount, layout->Mbr.Signature == UEFI_SIGNATURE));

		for (int i = 0; i < layout->PartitionCount; ++i)
		{
			m_PartitionTable->AddPartition(new MBRPartition(layout->PartitionEntry[i]));
		}
		break;
	case PARTITION_STYLE_GPT:
		m_PartitionTable = std::unique_ptr<PartitionTable>(new GPTPartitionTable(layout->PartitionCount, GuidToString(layout->Gpt.DiskId), layout->Gpt.StartingUsableOffset.QuadPart, layout->Gpt.UsableLength.QuadPart));
		for (int i = 0; i < layout->PartitionCount; ++i)
		{
			m_PartitionTable->AddPartition(new GPTPartition(layout->PartitionEntry[i]));
		}
		break;
	}
}

void Disk::ShowWidget()
{
	ImGui::Text("Disk %d - %s | Free Space: %d | Total Size: %d", m_Device.Index, m_Name.c_str(), m_FreeBytes, m_TotalBytes);
	ImGui::Text("Cylinders: %d", m_Cylinders);
	ImGui::Text("Tracks per Cylinder = %d", m_TracksPerCylinder);
	ImGui::Text("Sectors per Track = %d", m_SectorsPerTrack);
	ImGui::Text("Bytes per Sector = %d",m_BytesPerSector);
	ImGui::Text("Disk Size: %d GBs", m_TotalSize);
	m_PartitionTable->ShowWidget();
}

void Disk::GetWorkload()
{
	BOOL fResult;

	//GetDiskFreeSpaceEx function, which can get the space state of the drive disk, returns a BOOL-type data  
	fResult = GetDiskFreeSpaceEx(m_Name.c_str(),
		(PULARGE_INTEGER)&m_FreeBytesToCaller,
		(PULARGE_INTEGER)&m_TotalBytes,
		(PULARGE_INTEGER)&m_FreeBytes);

	if (fResult)
	{
		m_TotalBytes = m_TotalBytes / 1048576;
		m_FreeBytes = m_FreeBytes / 1048576;
	}
}

const std::string Disk::GetPhysicalName(unsigned long DriveIndex)
{
	CheckDriveIndex(DriveIndex);
	char path[24];
	sprintf_s(path, "\\\\.\\PHYSICALDRIVE%d", DriveIndex);
	return path;
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

const char* Disk::GetLogicalName(unsigned long DriveIndex, bool bKeepTrailingBackslash)
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

	for (int i = 0; hDrive == INVALID_HANDLE_VALUE; i++)
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
			printf_s("'%s' is not a GUID volume name\n", volume_name);
			continue;
		}

		drive_type = GetDriveTypeA(volume_name);
		volume_name[len - 1] = 0;

		if (QueryDosDeviceA(&volume_name[4], path, sizeof(path)) == 0)
		{
			printf_s("Failed to get device path for GUID volume '%s': %s\n", volume_name, GetLastError());
			continue;
		}

		int j;
		for (j = 0; (j < ARRAYSIZE(ignore_device)) && (_strnicmp(path, ignore_device[j], strlen(ignore_device[j])) != 0); j++);
		if (j < ARRAYSIZE(ignore_device))
		{
			printf_s("Skipping GUID volume for '%s'\n", path);
			continue;
		}

		// If we can't have FILE_SHARE_WRITE, forget it
		hDrive = CreateFileA(volume_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		if (hDrive == INVALID_HANDLE_VALUE) {
			printf_s("Could not open GUID volume '%s': %s\n", volume_name, GetLastError());
			continue;
		}

		if ((!DeviceIoControl(hDrive, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &DiskExtents, sizeof(DiskExtents), &size, NULL)) || (size <= 0))
		{
			printf_s("Could not get Disk Extents: %s\n", GetLastError());
			CloseHandle(hDrive);
			continue;
		}

		CloseHandle(hDrive);
		if ((DiskExtents.NumberOfDiskExtents >= 1) && (DiskExtents.Extents[0].DiskNumber == DriveIndex))
		{
			if (bKeepTrailingBackslash)
			{
				volume_name[len - 1] = '\\';
			}

			break;
		}
	}

	if (hVolume != INVALID_HANDLE_VALUE)
		FindVolumeClose(hVolume);

	return volume_name;
}

HANDLE Disk::GetHandle(const std::string& path)
{
	HANDLE hDrive = INVALID_HANDLE_VALUE;

	if (path != "")
	{
		hDrive = CreateFileA(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

		if (hDrive == INVALID_HANDLE_VALUE)
		{
			//printf_s("Could not open drive %s: %s\n", Path, GetLastError());
		}
	}

	return hDrive;
}