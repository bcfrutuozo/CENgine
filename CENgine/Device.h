#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <typeinfo>

#define DEVICE_MEMBERS_UNSIGNED_LONG \
	X(unsigned long, Address, 0) \
	X(unsigned long, Capabilities, 1) \
	X(unsigned long, ConfigFlags, 4) \

#define DEVICE_MEMBERS_STRING \
	X(std::wstring, ClassGUID, 2) \
	X(std::wstring, ContainerID, 5) \
	X(std::wstring, DeviceDesc, 6) \
	X(std::wstring, Driver, 7) \
	X(std::wstring, FriendlyName, 8) \
	X(std::wstring, LocationInformation, 10) \
	X(std::wstring, Mfg, 11) \
	X(std::wstring, ParentIdPrefix, 12) \
	X(std::wstring, Service, 13) \
	X(std::wstring, UINumberDescFormat, 14) \

#define DEVICE_MEMBERS_MULTIPLE_STRINGS \
	X(std::vector<std::wstring>, CompatibleIDs, 3) \
	X(std::vector<std::wstring>, HardwareID, 9) \


class Device
{
public:

	// Created 3 X Macro instances to avoid warnings during compilation

#define X(type, member) type member;
	DEVICE_MEMBERS_UNSIGNED_LONG
		DEVICE_MEMBERS_STRING
		DEVICE_MEMBERS_MULTIPLE_STRINGS
#undef X

		std::wstring Enumerator;
	unsigned int Index;
	bool IsLoaded = false;

	static unsigned int MembersCount()
	{
#define X(_, __) +1
		static const unsigned int count = DEVICE_MEMBERS_UNSIGNED_LONG + DEVICE_MEMBERS_STRING + DEVICE_MEMBERS_MULTIPLE_STRINGS;
#undef X

		return count;
	}

	static const std::wstring GetMemberName(int i)
	{
		switch (i)
		{
#define X(type, member, index) case index: return std::wstring(L#member);
			DEVICE_MEMBERS_UNSIGNED_LONG
				DEVICE_MEMBERS_STRING
				DEVICE_MEMBERS_MULTIPLE_STRINGS
#undef X
		}

		throw std::runtime_error("Invalid member driver type.");
	}

	static const type_info& GetMemberType(int i)
	{
		switch (i)
		{
#define X(type, member, index) case index: return typeid(type);
			DEVICE_MEMBERS_UNSIGNED_LONG
				DEVICE_MEMBERS_STRING
				DEVICE_MEMBERS_MULTIPLE_STRINGS
#undef X
		}

		throw std::runtime_error("Invalid member driver type.");
	}

	template<typename T>
	void SetMemberValue(int i, T value)
	{
		throw std::runtime_error("Invalid member data type.");
	}

	template<> void SetMemberValue<unsigned long>(int i, unsigned long value)
	{
		switch (i)
		{
#define X(type, member, index)  case index: member = value; break;
			DEVICE_MEMBERS_UNSIGNED_LONG
#undef X
		}
	};

	template<> void SetMemberValue<std::wstring>(int i, std::wstring value)
	{
		switch (i)
		{
#define X(type, member, index)  case index: member = value; break;
			DEVICE_MEMBERS_STRING
#undef X
		}
	};

	template<> void SetMemberValue<std::vector<std::wstring>>(int i, std::vector<std::wstring> value)
	{
		switch (i)
		{
#define X(type, member, index)  case index: member = value; break;
			DEVICE_MEMBERS_MULTIPLE_STRINGS
#undef X
		}
	};
};

struct DeviceEnumerator
{
	std::wstring path;
	unsigned int index;
};

#ifndef DVC_IMPL_SOURCE
#undef DEVICE_MEMBERS_UNSIGNED_LONG
#undef DEVICE_MEMBERS_STRING
#undef DEVICE_MEMBERS_MULTIPLE_STRINGS
#endif