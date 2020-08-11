#pragma once

#include "Device.h"
#include "CENgineException.h"
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

	// Exception
	class Exception : public CENgineException
	{
		using CENgineException::CENgineException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};

	// HrException
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};

	// Invalid Peripheral Index Exception
	class InvalidPeripheralIndexException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

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