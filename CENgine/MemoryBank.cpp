#include "MemoryBank.h"
#include "SMBIOS.h"
#include "Utilities.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

MemoryBank::MemoryBank(const TypeMemoryDevice& p_MemoryDevice)
	:
	m_DeviceLocator(p_MemoryDevice.DeviceLocator),
	m_BankLocator(static_cast<unsigned char>(std::stoi(SplitString(p_MemoryDevice.BankLocator, " ")[1]))),
	m_ErrorInformationHandle(p_MemoryDevice.ErrorInformationHandle),
	m_TotalWidth(p_MemoryDevice.TotalWidth),
	m_DeviceSet(p_MemoryDevice.DeviceSet),
	m_DataWidth(p_MemoryDevice.DataWidth),
	m_Speed(p_MemoryDevice.Speed),
	m_Manufacturer(p_MemoryDevice.Manufacturer),
	m_SerialNumber(p_MemoryDevice.SerialNumber),
	m_Type(p_MemoryDevice.MemoryType),
	m_AssetTagNumber(p_MemoryDevice.AssetTagNumber),
	m_PartNumber(p_MemoryDevice.PartNumber),
	m_Size(p_MemoryDevice.Size),
	m_FormFactor(p_MemoryDevice.FormFactor),
	m_TypeDetail(p_MemoryDevice.TypeDetail),
	m_Attributes(p_MemoryDevice.Attributes),
	m_ConfiguredClockSpeed(p_MemoryDevice.ConfiguredClockSpeed),
	m_ConfiguredVoltage(p_MemoryDevice.ConfiguredVoltage)
{
	m_Name = StringFormat("Physical Memory %u", m_BankLocator);
}

MemoryBank::~MemoryBank()
{}

void MemoryBank::Initialize()
{
	switch(m_Type)
	{
		case MB_TYPE_OTHER: m_VisualMemoryType = "Other"; break;
		case MB_TYPE_UNKNOWN: m_VisualMemoryType = "Unknown"; break;
		case MB_TYPE_DRAM: m_VisualMemoryType = "DRAM"; break;
		case MB_TYPE_EDRAM:	m_VisualMemoryType = "EDRAM"; break;
		case MB_TYPE_VRAM: m_VisualMemoryType = "VRAM"; break;
		case MB_TYPE_SRAM: m_VisualMemoryType = "SRAM"; break;
		case MB_TYPE_RAM: m_VisualMemoryType = "RAM"; break;
		case MB_TYPE_ROM: m_VisualMemoryType = "ROM"; break;
		case MB_TYPE_FLASH:	m_VisualMemoryType = "FLASH"; break;
		case MB_TYPE_EEPROM: m_VisualMemoryType = "EEPROM"; break;
		case MB_TYPE_FEPROM:m_VisualMemoryType = "FEPROM"; break;
		case MB_TYPE_EPROM:	m_VisualMemoryType = "EPROM"; break;
		case MB_TYPE_CDRAM:	m_VisualMemoryType = "CDRAM"; break;
		case MB_TYPE_3DRAM:	m_VisualMemoryType = "3DRAM"; break;
		case MB_TYPE_SDRAM:	m_VisualMemoryType = "SDRAM"; break;
		case MB_TYPE_SGRAM:	m_VisualMemoryType = "SGRAM"; break;
		case MB_TYPE_RDRAM:	m_VisualMemoryType = "RDRAM"; break;
		case MB_TYPE_DDR: m_VisualMemoryType = "DDR";	break;
		case MB_TYPE_DDR2: m_VisualMemoryType = "DDR2"; break;
		case MB_TYPE_DDR2FBDIMM: m_VisualMemoryType = "DDR2 FB-DIM";	break;
		case MB_TYPE_RESERVED: m_VisualMemoryType = "Reserved"; break;
		case MB_TYPE_DDR3: m_VisualMemoryType = "DDR3"; break;
		case MB_TYPE_FBD2: m_VisualMemoryType = "FBD2"; break;
		case MB_TYPE_DDR4: m_VisualMemoryType = "DDR4"; break;
		case MB_TYPE_LPDDR:	m_VisualMemoryType = "LPDDR"; break;
		case MB_TYPE_LPDDR2: m_VisualMemoryType = "LPDDR2"; break;
		case MB_TYPE_LPDDR3: m_VisualMemoryType = "LPDDR3"; break;
		case MB_TYPE_LPDDR4: m_VisualMemoryType = "LPDDR4"; break;
		case MB_TYPE_LOGICAL_NONVOLATILE_DEVICE: m_VisualMemoryType = "Logical non-volatile device"; break;
	}

	switch(m_FormFactor)
	{
		case MBFF_TYPE_OTHER: m_VisualFormFactor = "Other"; break;
		case MBFF_TYPE_UNKNOWN: m_VisualFormFactor = "Unknown"; break;
		case MBFF_TYPE_SIMM: m_VisualFormFactor = "SIMM"; break;
		case MBFF_TYPE_SIP:	m_VisualFormFactor = "SIP"; break;
		case MBFF_TYPE_CHIP:m_VisualFormFactor = "CHIP"; break;
		case MBFF_TYPE_DIP:	m_VisualFormFactor = "DIP"; break;
		case MBFF_TYPE_ZIP: m_VisualFormFactor = "ZIP"; break;
		case MBFF_TYPE_PROPRIETARYCARD: m_VisualFormFactor = "Proprietary Card"; break;
		case MBFF_TYPE_DIMM: m_VisualFormFactor = "DIMM"; break;
		case MBFF_TYPE_TSOP: m_VisualFormFactor = "TSOP"; break;
		case MBFF_TYPE_ROW: m_VisualFormFactor = "Row of chips"; break;
		case MBFF_TYPE_RIMM: m_VisualFormFactor = "RIMM"; break;
		case MBFF_TYPE_SODIMM: m_VisualFormFactor = "SODIMM"; break;
		case MBFF_TYPE_SRIMM: m_VisualFormFactor = "SRIMM"; break;
		case MBFF_TYPE_FBDIMM: m_VisualFormFactor = "FBDIMM"; break;
	}

	// If 0xFFFF appears for Total Width or Data Width, means that the memory size is unknown
	if(m_TotalWidth != 0xFFFF && m_DataWidth != 0xFFFF)
	{
		m_ErrorCorrectionBits = m_TotalWidth - m_DataWidth;
		m_HasErrorCorrectionBits = (m_ErrorCorrectionBits == 0) ? false : true;
	}

	m_IsOther = m_TypeDetail & (1 << 1);
	m_IsUnknown = m_TypeDetail & (1 << 2);
	m_IsFastPaged = m_TypeDetail & (1 << 3);
	m_IsStaticColumn = m_TypeDetail & (1 << 4);
	m_IsPseudoStatic = m_TypeDetail & (1 << 5);
	m_IsRAMBUS = m_TypeDetail & (1 << 6);
	m_IsSynchronous = m_TypeDetail & (1 << 7);
	m_IsCMOS = m_TypeDetail & (1 << 8);
	m_IsEDO = m_TypeDetail & (1 << 9);
	m_IsWindowDRAM = m_TypeDetail & (1 << 10);
	m_IsCacheDRAM = m_TypeDetail & (1 << 11);
	m_IsNonVolatile = m_TypeDetail & (1 << 12);
	m_IsRegistedBuffered = m_TypeDetail & (1 << 13);
	m_IsUnbufferedUnregisted = m_TypeDetail & (1 << 14);
	m_IsLRDIMM = m_TypeDetail & (1 << 15);
}

void MemoryBank::ShowWidget()
{
	if(ImGui::TreeNode(m_Name.c_str()))
	{
		ImGui::Text("Device Locator: %s", m_DeviceLocator.c_str());
		ImGui::Text("Bank: %u", m_BankLocator);
		ImGui::Text("Type: %s", m_VisualMemoryType.c_str());
		ImGui::Text("Form Factor: %s", m_VisualFormFactor.c_str());
		ImGui::Text("Speed: %u MHz", m_Speed);
		ImGui::Text("Manufacturer: %s", m_Manufacturer.c_str());
		ImGui::Text("Serial Number: %s", m_SerialNumber.c_str());
		ImGui::Text("Asset Tag Number: %s", m_AssetTagNumber.c_str());
		ImGui::Text("Part Number: %s", m_PartNumber.c_str());
		ImGui::Text("Size: %u MiB", m_Size);
		ImGui::Text("Configured Clock Speed: %u MHz", m_ConfiguredClockSpeed);
		ImGui::Text("Configured Voltage: %u mV", m_ConfiguredVoltage);
		if(m_IsOther || m_IsUnknown)
		{
			ImGui::Text("Unrecognized memory type details");
		}
		else
		{
			if(ImGui::TreeNode("Details"))
			{
				if(m_IsFastPaged)
				{
					ImGui::Text("Fast-paged");
				}
				if(m_IsStaticColumn)
				{
					ImGui::Text("Static column");
				}
				if(m_IsPseudoStatic)
				{
					ImGui::Text("Pseudo-static");
				}
				if(m_IsRAMBUS)
				{
					ImGui::Text("RAMBUS");
				}
				if(m_IsSynchronous)
				{
					ImGui::Text("Synchronous");
				}
				if(m_IsCMOS)
				{
					ImGui::Text("CMOS");
				}
				if(m_IsEDO)
				{
					ImGui::Text("EDO");
				}
				if(m_IsWindowDRAM)
				{
					ImGui::Text("Window DRAM");
				}
				if(m_IsCacheDRAM)
				{
					ImGui::Text("Cache DRAM");
				}
				if(m_IsNonVolatile)
				{
					ImGui::Text("Non-volatile");
				}
				if(m_IsRegistedBuffered)
				{
					ImGui::Text("Registered (Buffered)");
				}
				if(m_IsUnbufferedUnregisted)
				{
					ImGui::Text("Unbuffered (Unregistered)");
				}
				if(m_IsLRDIMM)
				{
					ImGui::Text("LRDIMM");
				}
				ImGui::TreePop();
			}
		}
		ImGui::Text("Attributes: %u", m_Attributes);
		// Heads the error information handle only if system provides it and if it's known
		if(m_ErrorInformationHandle < 0xFFFE)
		{
			ImGui::Text("Error Information Handle: %u", m_ErrorInformationHandle);
		}
		ImGui::Text("Total Width: %u", m_TotalWidth);
		ImGui::Text("Data Width: %u", m_DataWidth);
		if(m_HasErrorCorrectionBits)
		{
			ImGui::Text("Error Correction Bits: %u", m_ErrorCorrectionBits);
		}
		ImGui::Text("Device Set: %u", m_DeviceSet);
		ImGui::TreePop();
	}
}

void MemoryBank::GetWorkload()
{}