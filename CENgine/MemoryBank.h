#pragma once

#include "Peripheral.h"

struct TypeMemoryDevice;

#define MB_TYPE_OTHER						0x01
#define MB_TYPE_UNKNOWN						0x02
#define MB_TYPE_DRAM						0x03
#define MB_TYPE_EDRAM						0x04
#define MB_TYPE_VRAM						0x05
#define MB_TYPE_SRAM						0x06
#define MB_TYPE_RAM							0x07
#define MB_TYPE_ROM							0x08
#define MB_TYPE_FLASH						0x09
#define MB_TYPE_EEPROM						0x0A
#define MB_TYPE_FEPROM						0x0B
#define MB_TYPE_EPROM						0x0C
#define MB_TYPE_CDRAM						0x0D
#define MB_TYPE_3DRAM						0x0E
#define MB_TYPE_SDRAM						0x0F
#define MB_TYPE_SGRAM						0x10
#define MB_TYPE_RDRAM						0x11
#define MB_TYPE_DDR							0x12
#define MB_TYPE_DDR2						0x13
#define MB_TYPE_DDR2FBDIMM					0x14
#define MB_TYPE_RESERVED					0x15 | 0x16 | 0x17
#define MB_TYPE_DDR3						0x18
#define MB_TYPE_FBD2						0x19
#define MB_TYPE_DDR4						0x1A
#define MB_TYPE_LPDDR						0x1B
#define MB_TYPE_LPDDR2						0x1C
#define MB_TYPE_LPDDR3						0x1D
#define MB_TYPE_LPDDR4						0x1E
#define MB_TYPE_LOGICAL_NONVOLATILE_DEVICE  0x1F

#define MBFF_TYPE_OTHER				0x01	
#define MBFF_TYPE_UNKNOWN			0x02
#define MBFF_TYPE_SIMM				0x03
#define MBFF_TYPE_SIP				0x04
#define MBFF_TYPE_CHIP				0x05
#define MBFF_TYPE_DIP				0x06
#define MBFF_TYPE_ZIP				0x07
#define MBFF_TYPE_PROPRIETARYCARD	0x08
#define MBFF_TYPE_DIMM				0x09
#define MBFF_TYPE_TSOP				0x0A
#define MBFF_TYPE_ROW				0x0B
#define MBFF_TYPE_RIMM				0x0C
#define MBFF_TYPE_SODIMM			0x0D
#define MBFF_TYPE_SRIMM				0x0E
#define MBFF_TYPE_FBDIMM			0x0F

class MemoryBank : public Peripheral
{
public:

	MemoryBank(const TypeMemoryDevice& p_MemoryDevice);
	~MemoryBank();
	bool Initialize() override;
	void ShowWidget() override;
	void GetWorkload() override;
private:

	bool m_IsOther;
	bool m_IsUnknown;
	bool m_IsFastPaged;
	bool m_IsStaticColumn;
	bool m_IsPseudoStatic;
	bool m_IsRAMBUS;
	bool m_IsSynchronous;
	bool m_IsCMOS;
	bool m_IsEDO;
	bool m_IsWindowDRAM;
	bool m_IsCacheDRAM;
	bool m_IsNonVolatile;
	bool m_IsRegistedBuffered;
	bool m_IsUnbufferedUnregisted;
	bool m_IsLRDIMM;
	bool m_HasErrorCorrectionBits;
	unsigned char m_ErrorCorrectionBits;
	unsigned short m_ErrorInformationHandle;
	unsigned short m_TotalWidth;
	unsigned short m_DataWidth;
	unsigned short m_Size;
	unsigned char m_FormFactor;
	unsigned char m_DeviceSet;
	std::string m_DeviceLocator;
	unsigned char m_BankLocator;
	unsigned char m_MemoryType;
	unsigned char m_Type;
	std::string m_VisualFormFactor;
	std::string m_VisualMemoryType;
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