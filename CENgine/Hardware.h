#pragma once

#include "Peripheral.h"
#include "Disk.h"
#include "GPU.h"

#include <unordered_map>
#include <memory>

class Hardware
{
public:

	template<typename T>
	static const std::unique_ptr<T> GetDevice(int index)
	{
		if(typeid(T) == typeid(Disk))
		{
			return std::make_unique<T>(Peripheral::GetDeviceInformation<T>()[index]);
		}
		else if(typeid(T) == typeid(GPU))
		{
			return CreateGPU(Peripheral::GetDeviceInformation<T>()[index]);
		}

		throw std::runtime_error("Unsupported device type");
	}

	/*template<typename T>
	static const std::vector<std::unique_ptr<T>> GetDevices()
	{
		std::vector<std::unique_ptr<T>> c;
		if(typeid(T) == typeid(Disk))
		{
			std::vector<Device> dvc = Peripheral::GetDeviceInformation<T>();
			for(const auto& d : dvc)
			{
				c.push_back(std::make_unique<T>(d));
			}

			return c;
		}
		else if(typeid(T) == typeid(GPU))
		{

			std::vector<Device> dvc = Peripheral::GetDeviceInformation<T>();
			for(const auto& d : dvc)
			{
				c.push_back(CreateGPU(d));
			}

			return c;
		}

		throw std::runtime_error("Unsupported device type");
	}*/

	template<typename T>
	static const std::vector<std::unique_ptr<T>> GetDevices()
	{
		std::vector<std::unique_ptr<T>> c;
		std::vector<Device> dvc = Peripheral::GetDeviceInformation<T>();
		for(const auto& d : dvc)
		{
			c.emplace_back(std::make_unique<T>(d));
		}

		return c;
	}

	template<>
	static const std::vector<std::unique_ptr<GPU>> GetDevices()
	{
		std::vector<std::unique_ptr<GPU>> c;
		
		std::vector<Device> dvc = Peripheral::GetDeviceInformation<GPU>();
		for(const auto& d : dvc)
		{
			c.emplace_back(std::unique_ptr<GPU>(CreateGPU(d)));
		}

		return c;
	}

	template<typename T>
	static const unsigned long GetDeviceCount()
	{
		unsigned long count = 0;
		const auto& registryInformation = Peripheral::GetDeviceCount<T>();
		
		for(const auto& info : registryInformation)
		{
			count += info.second;
		}

		return count;
	}

private:

	static GPU* CreateGPU(Device device)
	{
		if(StringContainsInsensitive(device.Mfg, L"NVIDIA"))
		{
			return new NvidiaGPU(device);
		}
		else if(StringContainsInsensitive(device.Mfg, L"AMD"))
		{
			return new AmdGPU(device);
		}
		else if(StringContainsInsensitive(device.Mfg, L"INTEL"))
		{
			return new IntelGPU(device);
		}
	}
};