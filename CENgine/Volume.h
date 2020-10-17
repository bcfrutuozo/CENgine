#pragma once

#include "CENgineWin.h"
#include "Peripheral.h"
#include "Drive.h"

class Volume : public Peripheral
{
public:

	Volume(Device p_Device);
	~Volume();

	void Initialize() override;
	void GetWorkload() override;
	void ShowWidget() override;

	static const std::string FindVolumeByDriverEnumerator(const std::string& p_RegistryEnumerator, Drive::Type p_Type);
private:

	bool m_IsValidVolume;
	std::string m_ReadableName;
	std::string m_FileSystem;
	unsigned long long m_TotalSize;
	unsigned long long m_FreeSize;
	unsigned long long m_FreeBytesToCaller;

	static constexpr wchar_t MountsPoint[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2\\CPC\\Volume";
};