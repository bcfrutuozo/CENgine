#pragma once

#include <string>
#include <stdexcept>
#include <typeinfo>

#define DEVICE_MEMBERS_UNSIGNED_LONG \
	X(unsigned long, Address, 0) \
	X(unsigned long, Capabilities, 1) \
	X(unsigned long, ConfigFlags, 4) \

#define DEVICE_MEMBERS_STRING \
	X(std::string, ClassGUID, 2) \
	X(std::string, CompatibleIDs, 3) \
	X(std::string, ContainerID, 5) \
	X(std::string, DeviceDesc, 6) \
	X(std::string, Driver, 7) \
	X(std::string, FriendlyName, 8) \
	X(std::string, HardwareID, 9) \
	X(std::string, LocationInformation, 10) \
	X(std::string, Mfg, 11) \
	X(std::string, ParentIdPrefix, 12) \
	X(std::string, Service, 13) \
	X(std::string, UINumberDescFormat, 14)

struct Device
{
	// Allow peripheral to access private functions which set the device properties.
	friend class Peripheral;

	#define X(type, member) type member;
	DEVICE_MEMBERS_UNSIGNED_LONG
		DEVICE_MEMBERS_STRING
		#undef X

private:
	static unsigned int MembersCount()
	{
		#define X(_, __) +1
		static const unsigned int count = DEVICE_MEMBERS_UNSIGNED_LONG + DEVICE_MEMBERS_STRING;
		#undef X

		return count;
	}

	static const char* GetMemberName(int i)
	{
		switch(i)
		{
			#define X(type, member, index) case index: return #member;
			DEVICE_MEMBERS_UNSIGNED_LONG
				DEVICE_MEMBERS_STRING
				#undef X
		}

		throw std::runtime_error("Invalid member driver type.");
	}

	static const type_info& GetMemberType(int i)
	{
		switch(i)
		{
			#define X(type, member, index) case index: return typeid(type);
			DEVICE_MEMBERS_UNSIGNED_LONG
				DEVICE_MEMBERS_STRING
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
		switch(i)
		{
			#define X(type, member, index)  case index: member = value; break;
			DEVICE_MEMBERS_UNSIGNED_LONG
				#undef X
		}
	};

	template<> void SetMemberValue<std::string>(int i, std::string value)
	{
		switch(i)
		{
			#define X(type, member, index)  case index: member = value; break;
			DEVICE_MEMBERS_STRING
				#undef X
		}
	};
};

enum class Manufacturer
{
	UNSPECIFIED,
	AMD,
	NVIDIA,
};