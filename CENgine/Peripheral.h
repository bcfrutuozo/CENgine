#pragma once

#include "Device.h"
#include "Utilities.h"
#include "WinReg.h"

#include <cassert>
#include <unordered_map>
#include <sstream>
#include <memory>

class Hardware;
class Core;
class CPU;
class Disk;
class GPU;

class Peripheral
{
	friend Hardware;

public:

	virtual ~Peripheral() = default;
	virtual void Initialize() = 0;
	virtual void GetWorkload() = 0;
	virtual void ShowWidget() = 0;

protected:

	Peripheral()
		:
		m_Workload(0)
	{}

	Peripheral(Device device)
		:
		m_Device(device),
		m_Workload(0)
	{}

	Device m_Device;
	std::string m_Name;
	long m_Workload;
};