#pragma once

#include "Device.h"
#include "Utilities.h"
#include "WinReg.h"

#include <unordered_map>
#include <sstream>
#include <memory>

class Hardware;
class Disk;
class GPU;

class Peripheral
{
	friend Hardware;

public:

	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual const long GetWorkload() = 0;
	virtual ~Peripheral() = default;

protected:

	Device m_Device;
	long m_Workload;

private:

	template<typename T>
	static const std::vector<std::wstring> GetEnumerator()
	{
		static std::vector<std::wstring> DisksEnumerator{ L"disk" };
		static std::vector<std::wstring> GPUsEnumerator{ L"amdkmdag", L"nvlddmkm", L"iAlm" };

		if(typeid(T) == typeid(Disk))
		{
			return DisksEnumerator;
		}
		else if(typeid(T) == typeid(GPU))
		{
			return GPUsEnumerator;
		}
	}

	template<typename T>
	static const std::unordered_map<std::wstring, unsigned long> GetDeviceCount()
	{
		std::unordered_map<std::wstring, unsigned long> data;
		const auto& pathsEnum = GetEnumerator<T>();

		for(const auto& path : pathsEnum)
		{
			unsigned long amount = 0;
			RegKey key = { HKEY_LOCAL_MACHINE, RootServices, KEY_READ | KEY_WOW64_32KEY };
			if(key.TryOpen(key.Get(), path, KEY_READ | KEY_WOW64_32KEY))
			{
				if(key.TryOpen(key.Get(), L"Enum", KEY_READ | KEY_WOW64_32KEY))
				{
					amount += key.GetDwordValue(L"Count");
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}

			data[path] += amount;
		}

		return data;
	}

	template<typename T>
	static const std::vector<std::wstring> QueryRegistryForEnum(const std::unordered_map<std::wstring, unsigned long> registryInformation)
	{
		std::vector<std::wstring> paths;

		for(const auto& info : registryInformation)
		{
			for(unsigned int i = 0; i < info.second; i++)
			{
				RegKey key = { HKEY_LOCAL_MACHINE, RootServices, KEY_READ | KEY_WOW64_32KEY };
				if(key.TryOpen(key.Get(), info.first, KEY_READ | KEY_WOW64_32KEY))
				{
					if(key.TryOpen(key.Get(), L"Enum", KEY_READ | KEY_WOW64_32KEY))
					{
						std::wstringstream wss;
						wss << RootDeviceEnumerator << L"\\" << key.GetStringValue(std::to_wstring(i));
						paths.push_back(wss.str());
					}
				}
			}
		}

		return paths;
	}

	template<typename T>
	static const std::vector<Device> GetDeviceInformation()
	{
		std::vector<Device> vec;

		const auto& registryInformation = GetDeviceCount<T>();
		std::vector<std::wstring> paths = QueryRegistryForEnum<T>(registryInformation);

		for(const auto& path : paths)
		{
			RegKey key;

			key = { HKEY_LOCAL_MACHINE, path, KEY_READ | KEY_WOW64_32KEY };
			if(key.IsValid())
			{
				Device d;
				for(int m = 0; m < Device::MembersCount(); m++)
				{
					if(Device::GetMemberType(m) == typeid(unsigned long))
					{
						const auto& v = key.TryGetDwordValue(Device::GetMemberName(m));
						if(v.has_value())
						{
							d.SetMemberValue<unsigned long>(m, v.value());
						}
					}
					else if(Device::GetMemberType(m) == typeid(std::wstring))
					{
						const auto& v = key.TryGetStringValue(Device::GetMemberName(m));
						if(v.has_value())
						{
							d.SetMemberValue<std::wstring>(m, v.value());
						}
					}
					else if(Device::GetMemberType(m) == typeid(std::vector<std::wstring>))
					{
						const auto& v = key.TryGetMultiStringValue(Device::GetMemberName(m));
						if(v.has_value())
						{
							d.SetMemberValue<std::vector<std::wstring>>(m, v.value());
						}
					}
				}

				vec.push_back(d);
			}
		}

		return vec;
	};

	static constexpr wchar_t RootServices[] = L"SYSTEM\\CurrentControlSet\\Services";
	static constexpr wchar_t RootDeviceEnumerator[] = L"SYSTEM\\CurrentControlSet\\Enum";
};