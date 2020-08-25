#pragma once

#include "Peripheral.h"

struct TypeMemoryDevice;

class MemoryBank : public Peripheral
{
public:

	MemoryBank(const TypeMemoryDevice& p_MemoryDevice);
	~MemoryBank();
	void Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
private:

	unsigned short m_ErrorInformationHandle;
	unsigned short m_TotalWidth;
	unsigned short m_DataWidth;
	unsigned short m_Size;
	unsigned char m_FormFactor;
	unsigned char m_DeviceSet;
	std::string m_DeviceLocator;
	unsigned char m_BankLocator;
	unsigned char m_MemoryType;
	unsigned short m_TypeDetail;
	unsigned short m_Speed;
	std::string m_Manufacturer;
	std::string m_SerialNumber;
	std::string m_AssetTagNumber;
	std::string m_PartNumber;
	unsigned char m_Attributes;
	unsigned short m_ConfiguredClockSpeed;
	unsigned short m_ConfiguredVoltage;
};