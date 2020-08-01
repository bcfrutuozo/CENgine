#pragma once

#include "Peripheral.h"
#include "Core.h"
#include "CPU.h"
#include "Disk.h"
#include "GPU.h"
#include "NvidiaGPU.h"
#include "AmdGPU.h"
#include "IntelGPU.h"

#include <unordered_map>
#include <memory>

class Hardware
{
public:

	template<typename T>
	static const std::unique_ptr<T> GetDevice(int index = 0)
	{
		return std::make_unique<T>(Peripheral::GetDeviceInformation<T>()[index]);
	}

	template<>
	static const std::unique_ptr<CPU> GetDevice(int index)
	{
		std::vector<std::unique_ptr<Core>> cores;

		const auto& dvcCores = Peripheral::GetDeviceInformation<Core>();
		for(const auto& core : dvcCores)
		{
			cores.emplace_back(std::make_unique<Core>(core));
		}

		return std::make_unique<CPU>(std::move(cores));
	}

	template<>
	static const std::unique_ptr<GPU> GetDevice(int index)
	{
		return std::unique_ptr<GPU>(CreateGPU(Peripheral::GetDeviceInformation<GPU>()[index]));
	}

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
	static const std::vector<std::unique_ptr<CPU>> GetDevices()
	{
		throw std::runtime_error("Function GetDevices() not yet implemented for multiple CPUs");
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