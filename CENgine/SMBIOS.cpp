#include "SMBIOS.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <iomanip>	
#include <Windows.h>

#define DMI_READ_8U    *ptr_++
#define DMI_READ_16U   *((uint16_t*)ptr_), ptr_ += 2
#define DMI_READ_32U   *((uint32_t*)ptr_), ptr_ += 4
#define DMI_READ_64U   *((uint64_t*)ptr_), ptr_ += 8
#define DMI_ENTRY_HEADER_SIZE   4

struct RawSMBIOSData
{
	BYTE    Used20CallingMethod;
	BYTE    SMBIOSMajorVersion;
	BYTE    SMBIOSMinorVersion;
	BYTE    DmiRevision;
	DWORD   Length;
	BYTE    SMBIOSTableData[];
};

SMBIOS::SMBIOS()
{
	const BYTE byteSignature[] = { 'B', 'M', 'S', 'R' };
	const DWORD signature = *((DWORD*)byteSignature);

	// get the size of SMBIOS table
	DWORD size = GetSystemFirmwareTable(signature, 0, NULL, 0);
	if (size == 0)
	{
		//TODO:: THROW;
	}
	buffer.resize(size, 0);
	// retrieve the SMBIOS table

	if (size != GetSystemFirmwareTable(signature, 0, buffer.data(), size))
	{
		buffer.clear();
		//TODO:: THROW;
	}

	data_ = (&buffer.front() + 32);
	size_ = buffer.size();
	ptr_ = nullptr;
	version_ = 0;

	int vn = 0;

	// we have a valid SMBIOS entry point?

	if (data_[0] == '_' && data_[1] == 'S' && data_[2] == 'M' && data_[3] == '_')
	{
		// version 2.x

		// entry point length
		if (data_[5] != 0x1F)
		{
			throw;
		}
		// entry point revision
		if (data_[10] != 0)
		{
			throw;
		}
		// intermediate anchor string
		if (data_[16] != '_' || data_[17] != 'D' || data_[18] != 'M' || data_[19] != 'I' || data_[20] != '_')
		{
			throw;
		}

		// get the SMBIOS version
		vn = data_[6] << 8 | data_[7];
	}
	else if (data_[0] == '_' && data_[1] == 'S' && data_[2] == 'M' && data_[3] == '3' && data_[4] == '_')
	{
		// version 3.x

		// entry point length
		if (data_[6] != 0x18)
		{
			throw;
		}
		// entry point revision
		if (data_[10] != 0x01)
		{
			throw;
		}

		// get the SMBIOS version
		vn = data_[7] << 8 | data_[8];

	}
	RawSMBIOSData* smBiosData = NULL;
	smBiosData = (RawSMBIOSData*)&buffer.front();

	// get the SMBIOS version
	vn = smBiosData->SMBIOSMajorVersion << 8 | smBiosData->SMBIOSMinorVersion;
	data_ = smBiosData->SMBIOSTableData;
	size_ = smBiosData->Length;

	if (version_ == 0)
	{
		version_ = SMBIOS_3_0;
	}

	if (version_ > vn)
	{
		version_ = vn;
	}

	// is a valid version?
	if ((version_ < SMBIOS_2_0 || version_ > SMBIOS_2_8) && version_ != SMBIOS_3_0)
	{
		return;
	}

	Reset();
}


const char* SMBIOS::getString(int index) const
{
	if (index <= 0) return "";

	const char* ptr = (const char*)start_ + (size_t)entry_.length - DMI_ENTRY_HEADER_SIZE;
	for (int i = 1; i < index; ++i)
	{
		// TODO: check buffer limits
		while (*ptr != 0) ++ptr;
		++ptr;
	}
	return ptr;
}

void SMBIOS::Reset()
{
	ptr_ = start_ = nullptr;
}

bool SMBIOS::Increment()
{
	// jump to the next field
	if (ptr_ == nullptr)
	{
		ptr_ = start_ = data_;
	}
	else
	{
		ptr_ = start_ + entry_.length - DMI_ENTRY_HEADER_SIZE;
		while (ptr_ < data_ + size_ - 1 && !(ptr_[0] == 0 && ptr_[1] == 0)) ++ptr_;
		ptr_ += 2;
		if (ptr_ >= data_ + size_)
		{
			ptr_ = start_ = nullptr;
			return false;
		}
	}

	return true;
}

const Entry* SMBIOS::Next()
{
	if (data_ == nullptr)
	{
		return nullptr;
	}

	if (!Increment())
	{
		return nullptr;
	}

	memset(&entry_, 0, sizeof(entry_));

	// entry header
	entry_.type = DMI_READ_8U;
	entry_.length = DMI_READ_8U;
	entry_.handle = DMI_READ_16U;
	start_ = ptr_;

	if (entry_.type == DMI_END_OF_TABLE)
	{
		Reset();
		return nullptr;
	}

	return ParseEntry();
}

const Entry* SMBIOS::ParseEntry()
{
	std::vector<const char*> strings;

	switch (entry_.type)
	{
	case DMI_TYPE_BIOS:
		if (version_ >= SMBIOS_2_0)
		{
			entry_.data.bios.Vendor_ = DMI_READ_8U;
			entry_.data.bios.BIOSVersion_ = DMI_READ_8U;
			entry_.data.bios.BIOSStartingSegment = DMI_READ_16U;
			entry_.data.bios.BIOSReleaseDate_ = DMI_READ_8U;
			entry_.data.bios.BIOSROMSize = DMI_READ_8U;
			for (size_t i = 0; i < 8; ++i)
			{
				entry_.data.bios.BIOSCharacteristics[i] = DMI_READ_8U;
			}
			entry_.data.bios.Vendor = getString(entry_.data.bios.Vendor_);
			entry_.data.bios.BIOSVersion = getString(entry_.data.bios.BIOSVersion_);
			entry_.data.bios.BIOSReleaseDate = getString(entry_.data.bios.BIOSReleaseDate_);
		}
		if (version_ >= SMBIOS_2_4)
		{
			entry_.data.bios.ExtensionByte1 = DMI_READ_8U;
			entry_.data.bios.ExtensionByte2 = DMI_READ_8U;
			entry_.data.bios.SystemBIOSMajorRelease = DMI_READ_8U;
			entry_.data.bios.SystemBIOSMinorRelease = DMI_READ_8U;
			entry_.data.bios.EmbeddedFirmwareMajorRelease = DMI_READ_8U;
			entry_.data.bios.EmbeddedFirmwareMinorRelease = DMI_READ_8U;
		}
		break;
	case DMI_TYPE_SYSINFO:
		if (version_ >= SMBIOS_2_0)
		{
			entry_.data.sysinfo.Manufacturer_ = DMI_READ_8U;
			entry_.data.sysinfo.ProductName_ = DMI_READ_8U;
			entry_.data.sysinfo.Version_ = DMI_READ_8U;
			entry_.data.sysinfo.SerialNumber_ = DMI_READ_8U;
			entry_.data.sysinfo.Manufacturer = getString(entry_.data.sysinfo.Manufacturer_);
			entry_.data.sysinfo.ProductName = getString(entry_.data.sysinfo.ProductName_);
			entry_.data.sysinfo.Version = getString(entry_.data.sysinfo.Version_);
			entry_.data.sysinfo.SerialNumber = getString(entry_.data.sysinfo.SerialNumber_);
		}
		if (version_ >= SMBIOS_2_1)
		{
			for (int i = 0; i < 16; ++i)
			{
				entry_.data.sysinfo.UUID[i] = DMI_READ_8U;
			}

			entry_.data.sysinfo.WakeupType = DMI_READ_8U;
		}
		if (version_ >= SMBIOS_2_4)
		{
			entry_.data.sysinfo.SKUNumber_ = DMI_READ_8U;
			entry_.data.sysinfo.Family_ = DMI_READ_8U;

			entry_.data.sysinfo.SKUNumber = getString(entry_.data.sysinfo.SKUNumber_);
			entry_.data.sysinfo.Family = getString(entry_.data.sysinfo.Family_);
		}
		break;
	case DMI_TYPE_BASEBOARD:
		if (version_ >= SMBIOS_2_0)
		{
			entry_.data.baseboard.Manufacturer_ = DMI_READ_8U;
			entry_.data.baseboard.Product_ = DMI_READ_8U;
			entry_.data.baseboard.Version_ = DMI_READ_8U;
			entry_.data.baseboard.SerialNumber_ = DMI_READ_8U;
			entry_.data.baseboard.AssetTag_ = DMI_READ_8U;
			entry_.data.baseboard.FeatureFlags = DMI_READ_8U;
			entry_.data.baseboard.LocationInChassis_ = DMI_READ_8U;
			entry_.data.baseboard.ChassisHandle = DMI_READ_16U;
			entry_.data.baseboard.BoardType = DMI_READ_8U;
			entry_.data.baseboard.NoOfContainedObjectHandles = DMI_READ_8U;
			entry_.data.baseboard.ContainedObjectHandles = (uint16_t*)ptr_;
			ptr_ += entry_.data.baseboard.NoOfContainedObjectHandles * sizeof(uint16_t);
			entry_.data.baseboard.Manufacturer = getString(entry_.data.baseboard.Manufacturer_);
			entry_.data.baseboard.Product = getString(entry_.data.baseboard.Product_);
			entry_.data.baseboard.Version = getString(entry_.data.baseboard.Version_);
			entry_.data.baseboard.SerialNumber = getString(entry_.data.baseboard.SerialNumber_);
			entry_.data.baseboard.AssetTag = getString(entry_.data.baseboard.AssetTag_);
			entry_.data.baseboard.LocationInChassis = getString(entry_.data.baseboard.LocationInChassis_);
		}
		break;
	case DMI_TYPE_SYSENCLOSURE:
		if (version_ >= SMBIOS_2_0)
		{
			entry_.data.sysenclosure.Manufacturer_ = DMI_READ_8U;
			entry_.data.sysenclosure.Type = DMI_READ_8U;
			entry_.data.sysenclosure.Version_ = DMI_READ_8U;
			entry_.data.sysenclosure.SerialNumber_ = DMI_READ_8U;
			entry_.data.sysenclosure.AssetTag_ = DMI_READ_8U;

			entry_.data.sysenclosure.Manufacturer = getString(entry_.data.sysenclosure.Manufacturer_);
			entry_.data.sysenclosure.Version = getString(entry_.data.sysenclosure.Version_);
			entry_.data.sysenclosure.SerialNumber = getString(entry_.data.sysenclosure.SerialNumber_);
			entry_.data.sysenclosure.AssetTag = getString(entry_.data.sysenclosure.AssetTag_);
		}
		if (version_ >= SMBIOS_2_1)
		{
			entry_.data.sysenclosure.BootupState = DMI_READ_8U;
			entry_.data.sysenclosure.PowerSupplyState = DMI_READ_8U;
			entry_.data.sysenclosure.ThermalState = DMI_READ_8U;
			entry_.data.sysenclosure.SecurityStatus = DMI_READ_8U;
		}
		if (version_ >= SMBIOS_2_3)
		{
			entry_.data.sysenclosure.OEMdefined = DMI_READ_32U;
			entry_.data.sysenclosure.Height = DMI_READ_8U;
			entry_.data.sysenclosure.NumberOfPowerCords = DMI_READ_8U;
			entry_.data.sysenclosure.ContainedElementCount = DMI_READ_8U;
			entry_.data.sysenclosure.ContainedElementRecordLength = DMI_READ_8U;
			entry_.data.sysenclosure.ContainedElements = ptr_;
			ptr_ += entry_.data.sysenclosure.ContainedElementCount * entry_.data.sysenclosure.ContainedElementRecordLength;
		}
		if (version_ >= SMBIOS_2_7)
		{
			entry_.data.sysenclosure.SKUNumber_ = DMI_READ_8U;

			entry_.data.sysenclosure.SKUNumber = getString(entry_.data.sysenclosure.SKUNumber_);
		}
		break;
	case DMI_TYPE_PROCESSOR:
		if (version_ >= SMBIOS_2_0)
		{
			entry_.data.processor.SocketDesignation_ = DMI_READ_8U;
			entry_.data.processor.ProcessorType = DMI_READ_8U;
			entry_.data.processor.ProcessorFamily = DMI_READ_8U;
			entry_.data.processor.ProcessorManufacturer_ = DMI_READ_8U;
			for (int i = 0; i < 8; ++i)
			{
				entry_.data.processor.ProcessorID[i] = DMI_READ_8U;
			}
			entry_.data.processor.ProcessorVersion_ = DMI_READ_8U;
			entry_.data.processor.Voltage = DMI_READ_8U;
			entry_.data.processor.ExternalClock = DMI_READ_16U;
			entry_.data.processor.MaxSpeed = DMI_READ_16U;
			entry_.data.processor.CurrentSpeed = DMI_READ_16U;
			entry_.data.processor.Status = DMI_READ_8U;
			entry_.data.processor.ProcessorUpgrade = DMI_READ_8U;

			entry_.data.processor.SocketDesignation = getString(entry_.data.processor.SocketDesignation_);
			entry_.data.processor.ProcessorManufacturer = getString(entry_.data.processor.ProcessorManufacturer_);
			entry_.data.processor.ProcessorVersion = getString(entry_.data.processor.ProcessorVersion_);
		}
		if (version_ >= SMBIOS_2_1)
		{
			entry_.data.processor.L1CacheHandle = DMI_READ_16U;
			entry_.data.processor.L2CacheHandle = DMI_READ_16U;
			entry_.data.processor.L3CacheHandle = DMI_READ_16U;
		}
		if (version_ >= SMBIOS_2_3)
		{
			entry_.data.processor.SerialNumber_ = DMI_READ_8U;
			entry_.data.processor.AssetTagNumber_ = DMI_READ_8U;
			entry_.data.processor.PartNumber_ = DMI_READ_8U;

			entry_.data.processor.SerialNumber = getString(entry_.data.processor.SerialNumber_);
			entry_.data.processor.AssetTagNumber = getString(entry_.data.processor.AssetTagNumber_);
			entry_.data.processor.PartNumber = getString(entry_.data.processor.PartNumber_);
		}
		if (version_ >= SMBIOS_2_5)
		{
			entry_.data.processor.CoreCount = DMI_READ_8U;
			entry_.data.processor.CoreEnabled = DMI_READ_8U;
			entry_.data.processor.ThreadCount = DMI_READ_8U;
			entry_.data.processor.ProcessorCharacteristics = DMI_READ_16U;
		}
		if (version_ >= SMBIOS_2_6)
		{
			entry_.data.processor.ProcessorFamily2 = DMI_READ_16U;
		}
		if (version_ >= SMBIOS_3_0)
		{
			entry_.data.processor.CoreCount2 = DMI_READ_16U;
			entry_.data.processor.CoreEnabled2 = DMI_READ_16U;
			entry_.data.processor.ThreadCount2 = DMI_READ_16U;
		}

		break;
	case DMI_TYPE_SYSSLOT:
		if (version_ >= SMBIOS_2_0)
		{
			entry_.data.sysslot.SlotDesignation_ = DMI_READ_8U;
			entry_.data.sysslot.SlotType = DMI_READ_8U;
			entry_.data.sysslot.SlotDataBusWidth = DMI_READ_8U;
			entry_.data.sysslot.CurrentUsage = DMI_READ_8U;
			entry_.data.sysslot.SlotLength = DMI_READ_8U;
			entry_.data.sysslot.SlotID = DMI_READ_16U;
			entry_.data.sysslot.SlotCharacteristics1 = DMI_READ_8U;

			entry_.data.sysslot.SlotDesignation = getString(entry_.data.sysslot.SlotDesignation_);
		}
		if (version_ >= SMBIOS_2_1)
		{
			entry_.data.sysslot.SlotCharacteristics2 = DMI_READ_8U;
		}
		if (version_ >= SMBIOS_2_6)
		{
			entry_.data.sysslot.SegmentGroupNumber = DMI_READ_16U;
			entry_.data.sysslot.BusNumber = DMI_READ_8U;
			entry_.data.sysslot.DeviceOrFunctionNumber = DMI_READ_8U;
		}
		break;
	case DMI_TYPE_PHYSICAL_MEMORY_ARRAY:
		if (version_ >= SMBIOS_2_1)
		{
			entry_.data.physmem.Location = DMI_READ_8U;
			entry_.data.physmem.Use = DMI_READ_8U;
			entry_.data.physmem.ErrorCorrection = DMI_READ_8U;
			entry_.data.physmem.MaximumCapacity = DMI_READ_32U;
			entry_.data.physmem.ErrorInformationHandle = DMI_READ_16U;
			entry_.data.physmem.NumberDevices = DMI_READ_16U;
		}
		if (version_ >= SMBIOS_2_7)
		{
			entry_.data.physmem.ExtendedMaximumCapacity = DMI_READ_64U;
		}
		break;
	case DMI_TYPE_MEMORY_DEVICE:
		if (version_ >= SMBIOS_2_1)
		{
			entry_.data.memory.PhysicalArrayHandle = DMI_READ_16U;
			entry_.data.memory.ErrorInformationHandle = DMI_READ_16U;
			entry_.data.memory.TotalWidth = DMI_READ_16U;
			entry_.data.memory.DataWidth = DMI_READ_16U;
			entry_.data.memory.Size = DMI_READ_16U;
			entry_.data.memory.FormFactor = DMI_READ_8U;
			entry_.data.memory.DeviceSet = DMI_READ_8U;
			entry_.data.memory.DeviceLocator_ = DMI_READ_8U;
			entry_.data.memory.BankLocator_ = DMI_READ_8U;
			entry_.data.memory.MemoryType = DMI_READ_8U;
			entry_.data.memory.TypeDetail = DMI_READ_16U;
			entry_.data.memory.DeviceLocator = getString(entry_.data.memory.DeviceLocator_);
			entry_.data.memory.BankLocator = getString(entry_.data.memory.BankLocator_);
		}
		if (version_ >= SMBIOS_2_3)
		{
			entry_.data.memory.Speed = DMI_READ_16U;
			entry_.data.memory.Manufacturer_ = DMI_READ_8U;
			entry_.data.memory.SerialNumber_ = DMI_READ_8U;
			entry_.data.memory.AssetTagNumber_ = DMI_READ_8U;
			entry_.data.memory.PartNumber_ = DMI_READ_8U;
			entry_.data.memory.Manufacturer = getString(entry_.data.memory.Manufacturer_);
			entry_.data.memory.SerialNumber = getString(entry_.data.memory.SerialNumber_);
			entry_.data.memory.AssetTagNumber = getString(entry_.data.memory.AssetTagNumber_);
			entry_.data.memory.PartNumber = getString(entry_.data.memory.PartNumber_);
		}
		if (version_ >= SMBIOS_2_6)
		{
			entry_.data.memory.Attributes = DMI_READ_8U;
		}
		if (version_ >= SMBIOS_2_7)
		{
			entry_.data.memory.ExtendedSize = DMI_READ_32U;
			entry_.data.memory.ConfiguredClockSpeed = DMI_READ_16U;
		}
		if (version_ >= SMBIOS_2_8)
		{
			entry_.data.memory.MinimumVoltage = DMI_READ_16U;
			entry_.data.memory.MinimumVoltage = DMI_READ_16U;
			entry_.data.memory.ConfiguredVoltage = DMI_READ_16U;
		}
		break;
	}

	return &entry_;
}

const int SMBIOS::GetVersion() const
{
	return version_;
}

bool SMBIOS::IsValid() const
{
	return data_ != nullptr;
}

const TypeMemoryDevice SMBIOS::GetPhysicalMemory(const int p_Index)
{
	const Entry* entry = NULL;
	int memIndex = 0;
	while (true)
	{
		entry = Next();
		if (entry == nullptr)
			break;

		if (entry->type == DMI_TYPE_MEMORY_DEVICE)
		{
			if (memIndex == p_Index)
			{
				Reset();
				return entry->data.memory;
			}
			else
			{
				memIndex++;
			}
		}
	}
}

const TypePhysicalMemory SMBIOS::GetPhysicalMemoryArray()
{
	const Entry* entry = NULL;
	while (true)
	{
		int memIndex = 0;
		entry = Next();
		if (entry == nullptr)
			break;

		if (entry->type == DMI_TYPE_PHYSICAL_MEMORY_ARRAY)
		{
			Reset();
			return entry->data.physmem;
		}
	}
}

bool SMBIOS::Print()
{
	std::ostringstream output;
	int vers = GetVersion();
	const Entry* entry = NULL;
	while (true)
	{
		entry = Next();
		if (entry == NULL) break;

		output << "Handle 0x" << std::hex << std::setw(4) << std::setfill('0') << (int)entry->handle << std::dec
			<< ", DMI Type " << (int)entry->type << ", " << (int)entry->length << " bytes\n";

		if (entry->type == DMI_TYPE_BIOS)
		{
			if (vers >= SMBIOS_2_0)
			{
				output << "                      Vendor: " << entry->data.bios.Vendor << '\n';
				output << "                 BIOSVersion: " << entry->data.bios.BIOSVersion << '\n';
				output << "         BIOSStartingSegment: 0x" << std::hex << (int)entry->data.bios.BIOSStartingSegment << std::dec << '\n';
				output << "             BIOSReleaseDate: " << entry->data.bios.BIOSReleaseDate << '\n';
				output << "                 BIOSROMSize: " << (((int)entry->data.bios.BIOSROMSize + 1) * 64) << " KiB \n";
			}
			if (vers >= SMBIOS_2_4)
			{
				output << "      SystemBIOSMajorRelease: " << (int)entry->data.bios.SystemBIOSMajorRelease << '\n';
				output << "      SystemBIOSMinorRelease: " << (int)entry->data.bios.SystemBIOSMinorRelease << '\n';
				output << "EmbeddedFirmwareMajorRelease: " << (int)entry->data.bios.EmbeddedFirmwareMajorRelease << '\n';
				output << "EmbeddedFirmwareMinorRelease: " << (int)entry->data.bios.EmbeddedFirmwareMinorRelease << '\n';
			}
			output << '\n';
		}
		else
			if (entry->type == DMI_TYPE_SYSINFO)
			{
				if (vers >= SMBIOS_2_0)
				{
					output << "     Manufacturer: " << entry->data.sysinfo.Manufacturer << '\n';
					output << "      ProductName: " << entry->data.sysinfo.ProductName << '\n';
					output << "          Version: " << entry->data.sysinfo.Version << '\n';
					output << "     SerialNumber: " << entry->data.sysinfo.SerialNumber << '\n';
				}
				if (vers >= SMBIOS_2_1)
				{
					output << "             UUID: ";

					for (size_t i = 0; i < 16; ++i)
					{
						output << std::hex << std::setw(2) << std::setfill('0') << (int)entry->data.sysinfo.UUID[i] << ' ';
					}

					output << '\n' << std::dec;
				}
				if (vers >= SMBIOS_2_4)
				{
					output << "        SKUNumber: " << entry->data.sysinfo.SKUNumber << '\n';
					output << "           Family: " << entry->data.sysinfo.Family << '\n';
				}
				output << '\n';
			}
			else
				if (entry->type == DMI_TYPE_BASEBOARD)
				{
					if (vers >= SMBIOS_2_0)
					{
						output << "     Manufacturer: " << entry->data.baseboard.Manufacturer << '\n';
						output << "          Product: " << entry->data.baseboard.Product << '\n';
						output << "          Version: " << entry->data.baseboard.Version << '\n';
						output << "     SerialNumber: " << entry->data.baseboard.SerialNumber << '\n';
						output << "         AssetTag: " << entry->data.baseboard.AssetTag << '\n';
						output << "LocationInChassis: " << entry->data.baseboard.LocationInChassis << '\n';
						output << "    ChassisHandle: " << entry->data.baseboard.ChassisHandle << '\n';
						output << "        BoardType: " << (int)entry->data.baseboard.BoardType << '\n';
					}
					output << '\n';
				}
				else
					if (entry->type == DMI_TYPE_SYSENCLOSURE)
					{
						if (vers >= SMBIOS_2_0)
						{
							output << "     Manufacturer: " << entry->data.sysenclosure.Manufacturer << '\n';
							output << "          Version: " << entry->data.sysenclosure.Version << '\n';
							output << "     SerialNumber: " << entry->data.sysenclosure.SerialNumber << '\n';
							output << "         AssetTag: " << entry->data.sysenclosure.AssetTag << "\n";
						}
						if (vers >= SMBIOS_2_3)
						{
							output << "  Contained Count: " << (int)entry->data.sysenclosure.ContainedElementCount << '\n';
							output << " Contained Length: " << (int)entry->data.sysenclosure.ContainedElementRecordLength << '\n';
						}
						if (vers >= SMBIOS_2_7)
						{
							output << "        SKUNumber: " << entry->data.sysenclosure.SKUNumber << '\n';
						}
						output << '\n';
					}
					else
						if (entry->type == DMI_TYPE_PROCESSOR)
						{
							if (vers >= SMBIOS_2_0)
							{
								output << "    SocketDesignation: " << entry->data.processor.SocketDesignation << '\n';
								output << "      ProcessorFamily: " << (int)entry->data.processor.ProcessorFamily << '\n';
								output << "ProcessorManufacturer: " << entry->data.processor.ProcessorManufacturer << '\n';
								output << "     ProcessorVersion: " << entry->data.processor.ProcessorVersion << '\n';
								output << "          ProcessorID: ";
								for (size_t i = 0; i < 8; ++i)
									output << std::hex << std::setw(2) << std::setfill('0') << (int)entry->data.processor.ProcessorID[i] << ' ';
								output << std::dec << '\n';
							}
							if (vers >= SMBIOS_2_5)
							{
								output << "            CoreCount: " << (int)entry->data.processor.CoreCount << '\n';
								output << "          CoreEnabled: " << (int)entry->data.processor.CoreEnabled << '\n';
								output << "          ThreadCount: " << (int)entry->data.processor.ThreadCount << '\n';
							}
							if (vers >= SMBIOS_2_6)
							{
								output << "     ProcessorFamily2: " << entry->data.processor.ProcessorFamily2 << '\n';
							}
							output << '\n';
						}
						else
							if (entry->type == DMI_TYPE_SYSSLOT)
							{
								if (vers >= SMBIOS_2_0)
								{
									output << "    SlotDesignation: " << entry->data.sysslot.SlotDesignation << '\n';
									output << "           SlotType: " << (int)entry->data.sysslot.SlotType << '\n';
									output << "   SlotDataBusWidth: " << (int)entry->data.sysslot.SlotDataBusWidth << '\n';
									output << "             SlotID: " << (int)entry->data.sysslot.SlotID << '\n';
								}
								if (vers >= SMBIOS_2_6)
								{
									output << " SegmentGroupNumber: " << entry->data.sysslot.SegmentGroupNumber << '\n';
									output << "          BusNumber: " << (int)entry->data.sysslot.BusNumber << '\n';
								}
								output << '\n';
							}
							else
								if (entry->type == DMI_TYPE_PHYSICAL_MEMORY_ARRAY)
								{
									if (vers >= SMBIOS_2_1)
									{
										output << "                  Use: 0x" << std::hex << (int)entry->data.physmem.Use << std::dec << '\n';
										output << "        NumberDevices: " << entry->data.physmem.NumberDevices << '\n';
										output << "      MaximumCapacity: " << entry->data.physmem.MaximumCapacity << " KiB\n";
										output << "   ExtMaximumCapacity: " << entry->data.physmem.ExtendedMaximumCapacity << " KiB\n";
									}
									output << '\n';
								}
								else
									if (entry->type == DMI_TYPE_MEMORY_DEVICE)
									{
										if (vers >= SMBIOS_2_1)
										{
											output << "        DeviceLocator: " << entry->data.memory.DeviceLocator << '\n';
											output << "          BankLocator: " << entry->data.memory.BankLocator << '\n';
										}
										if (vers >= SMBIOS_2_3)
										{
											output << "                Speed: " << entry->data.memory.Speed << " MHz\n";
											output << "         Manufacturer: " << entry->data.memory.Manufacturer << '\n';
											output << "         SerialNumber: " << entry->data.memory.SerialNumber << '\n';
											output << "       AssetTagNumber: " << entry->data.memory.AssetTagNumber << '\n';
											output << "           PartNumber: " << entry->data.memory.PartNumber << '\n';
											output << "                 Size: " << entry->data.memory.Size << " MiB\n";
											output << "         ExtendedSize: " << entry->data.memory.ExtendedSize << " MiB\n";
										}
										if (vers >= SMBIOS_2_7)
										{
											output << " ConfiguredClockSpeed: " << entry->data.memory.ConfiguredClockSpeed << " MHz\n";
										}
										output << '\n';
									}
	}

	return true;
}