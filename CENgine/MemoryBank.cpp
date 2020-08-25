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
{
}

void MemoryBank::Initialize()
{
}

void MemoryBank::ShowWidget()
{
	if(ImGui::TreeNode(m_Name.c_str()))
	{
		ImGui::Text("Device Locator: %s", m_DeviceLocator.c_str());
		ImGui::Text("Bank: %u", m_BankLocator);
		ImGui::Text("Speed: %u MHz", m_Speed);
		ImGui::Text("Manufacturer: %s", m_Manufacturer.c_str());
		ImGui::Text("Serial Number: %s", m_SerialNumber.c_str());
		ImGui::Text("Asset Tag Number: %s", m_AssetTagNumber.c_str());
		ImGui::Text("Part Number: %s", m_PartNumber.c_str());
		ImGui::Text("Size: %u MiB", m_Size);
		ImGui::Text("Configured Clock Speed: %u MHz", m_ConfiguredClockSpeed);
		ImGui::Text("Configured Voltage: %u mV", m_ConfiguredVoltage);
		ImGui::Text("Form Factor: %u", m_FormFactor);
		ImGui::Text("Type Detail: %u", m_TypeDetail);
		ImGui::Text("Attributes: %u", m_Attributes);
		ImGui::Text("Error Information Handle: %u", m_ErrorInformationHandle);
		ImGui::Text("Total Width: %u", m_TotalWidth);
		ImGui::Text("Data Width: %u", m_DataWidth);
		ImGui::Text("Device Set: %u", m_DeviceSet);
		ImGui::TreePop();
	}
}

void MemoryBank::GetWorkload()
{
}
