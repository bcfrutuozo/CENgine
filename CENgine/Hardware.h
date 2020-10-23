#pragma once

#include "Peripheral.h"
#include "Core.h"
#include "CPU.h"
#include "Drive.h"
#include "GPU.h"
#include "NvidiaGPU.h"
#include "AmdGPU.h"
#include "IntelGPU.h"
#include "Volume.h"
#include "Memory.h"
#include "SMBIOS.h"

#include <unordered_map>

class Hardware
{
public:

	template<typename T>
	static const std::unique_ptr<T> GetDevice(int index = 0)
	{
		return std::make_unique<T>(GetDeviceInformation<T>()[index]);
	}

	template<>
	static const std::unique_ptr<CPU> GetDevice(int index)
	{
		std::vector<std::unique_ptr<Core>> cores;

		const auto& dvcCores = GetDeviceInformation<Core>();
		for (const auto& core : dvcCores)
		{
			cores.emplace_back(std::make_unique<Core>(core));
		}

		return std::make_unique<CPU>(std::move(cores));
	}

	template<>
	static const std::unique_ptr<Memory> GetDevice(int index)
	{
		std::vector<std::unique_ptr<MemoryBank>> c;

		//Physical Memory has no device in registry. Hidden by SMBIOS
		const auto memArray = GetSMBIOSInstance().GetPhysicalMemoryArray();
		for (int i = 0; i < memArray.NumberDevices; i++)
		{
			c.emplace_back(std::make_unique<MemoryBank>(GetSMBIOSInstance().GetPhysicalMemory(i)));
		}

		return std::make_unique<Memory>(memArray, std::move(c));;
	}

	template<typename T>
	static const std::vector<std::unique_ptr<T>> GetDevices()
	{
		std::vector<std::unique_ptr<T>> c;

		const auto& dvc = GetDeviceInformation<T>();
		for (const auto& d : dvc)
		{
			c.emplace_back(std::make_unique<T>(d));
		}

		return c;
	}

	template<>
	static const std::vector<std::unique_ptr<CPU>> GetDevices()
	{
		throw std::runtime_error("Function GetDevices() not yet implemented for multiple CPUs");
	}

	template<>
	static const std::vector<std::unique_ptr<GPU>> GetDevices()
	{
		std::vector<std::unique_ptr<GPU>> c;

		std::vector<Device> dvc = GetDeviceInformation<GPU>();
		for (const auto& d : dvc)
		{
			c.emplace_back(std::unique_ptr<GPU>(CreateGPU(d)));
		}

		return c;
	}

private:

	static SMBIOS& GetSMBIOSInstance()
	{
		static SMBIOS instance;
		return instance;
	}

	static GPU* CreateGPU(Device device)
	{
		if (StringContainsInsensitive(device.Mfg, L"NVIDIA"))
		{
			return new NvidiaGPU(device);
		}
		else if (StringContainsInsensitive(device.Mfg, L"AMD"))
		{
			return new AmdGPU(device);
		}
		else if (StringContainsInsensitive(device.Mfg, L"INTEL"))
		{
			return new IntelGPU(device);
		}

		throw std::runtime_error("Invalid GPU type passed to factory");
	}
	static constexpr wchar_t RootServices[] = L"SYSTEM\\CurrentControlSet\\Services";
	static constexpr wchar_t RootDeviceEnumerator[] = L"SYSTEM\\CurrentControlSet\\Enum";

	template<typename T>
	static const std::vector<std::wstring> GetEnumerator()
	{
		// Use ACPI/PPM to get the real number of cores
		static std::vector<std::wstring> CoresEnumerator{ L"AmdPPM", L"intelppm" };
		static std::vector<std::wstring> DisksEnumerator{ L"disk", L"cdrom" };
		static std::vector<std::wstring> GPUsEnumerator{ L"amdkmdag", L"nvlddmkm", L"iAlm" };
		static std::vector<std::wstring> VolumesEnumerator{ L"volume" };

		if (typeid(T) == typeid(Core))
		{
			return CoresEnumerator;
		}
		else if (typeid(T) == typeid(Drive))
		{
			return DisksEnumerator;
		}
		else if (typeid(T) == typeid(GPU))
		{
			return GPUsEnumerator;
		}
		else if (typeid(T) == typeid(Volume))
		{
			return VolumesEnumerator;
		}

		throw std::runtime_error("Invalid type to get enumerator");
	}

	template<typename T>
	static const std::unordered_map<std::wstring, unsigned long> GetDeviceCount()
	{
		std::unordered_map<std::wstring, unsigned long> data;
		const auto& pathsEnum = GetEnumerator<T>();

		for (const auto& path : pathsEnum)
		{
			unsigned long amount = 0;
			RegKey key = { HKEY_LOCAL_MACHINE, RootServices, KEY_READ | KEY_WOW64_32KEY };
			if (key.TryOpen(key.Get(), path, KEY_READ | KEY_WOW64_32KEY))
			{
				if (key.TryOpen(key.Get(), L"Enum", KEY_READ | KEY_WOW64_32KEY))
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
	static const std::vector<DeviceEnumerator> QueryRegistryForEnum(const std::unordered_map<std::wstring, unsigned long> registryInformation)
	{
		std::vector<DeviceEnumerator> deviceEnumerator;

		for (const auto& info : registryInformation)
		{
			for (unsigned int i = 0; i < info.second; i++)
			{
				RegKey key = { HKEY_LOCAL_MACHINE, RootServices, KEY_READ | KEY_WOW64_32KEY };
				if (key.TryOpen(key.Get(), info.first, KEY_READ | KEY_WOW64_32KEY))
				{
					if (key.TryOpen(key.Get(), L"Enum", KEY_READ | KEY_WOW64_32KEY))
					{
						DeviceEnumerator dm;
						std::wstringstream wss;
						wss << RootDeviceEnumerator << L"\\" << key.GetStringValue(std::to_wstring(i));
						dm.index = i;
						dm.path = wss.str();
						deviceEnumerator.push_back(dm);
					}
				}
			}
		}

		return deviceEnumerator;
	}

	template<typename T>
	static const std::vector<Device> GetDeviceInformation()
	{
		std::vector<Device> vec;

		const auto& registryInformation = GetDeviceCount<T>();
		const auto& deviceEnumerator = QueryRegistryForEnum<T>(registryInformation);

		int index = 0;
		for (const auto& dm : deviceEnumerator)
		{
			RegKey key;

			key = { HKEY_LOCAL_MACHINE, dm.path, KEY_READ | KEY_WOW64_32KEY };
			if (key.IsValid())
			{
				Device d;

				d.Enumerator = std::wstring(dm.path);

				for (unsigned int m = 0; m < Device::MembersCount(); m++)
				{
					if (Device::GetMemberType(m) == typeid(unsigned long))
					{
						const auto& v = key.TryGetDwordValue(Device::GetMemberName(m));
						if (v.has_value())
						{
							d.SetMemberValue<unsigned long>(m, v.value());
						}
					}
					else if (Device::GetMemberType(m) == typeid(std::wstring))
					{
						const auto& v = key.TryGetStringValue(Device::GetMemberName(m));
						if (v.has_value())
						{
							d.SetMemberValue<std::wstring>(m, v.value());
						}
					}
					else if (Device::GetMemberType(m) == typeid(std::vector<std::wstring>))
					{
						const auto& v = key.TryGetMultiStringValue(Device::GetMemberName(m));
						if (v.has_value())
						{
							d.SetMemberValue<std::vector<std::wstring>>(m, v.value());
						}
					}
				}

				d.Index = dm.index;
				d.IsLoaded = true;
				vec.push_back(d);
			}
		}

		return vec;
	};
};