#pragma once

#include "Device.h"
#include "Factory.h"
#include "Utilities.h"

#include <tchar.h>
#include <Windows.h>
#include <winreg.h>
#include <memory>

class Peripheral
{
public:

	virtual void Initialize()
	{ }

	virtual void Shutdown()
	{ }

	virtual const long GetWorkload()
	{
		return 0;
	}

	virtual ~Peripheral() = default;

	template<typename P>
	static std::unique_ptr<P> GetPeripheral()
	{
		std::unique_ptr<P> p;
		Device d;
		HKEY key = nullptr;
		HKEY key2 = nullptr;
		HKEY keyServ;
		HKEY keyEnum;
		LONG lResult;//LONG-Type Variable-Save Function Return Value  

		if(typeid(P) == typeid(GPU))
		{
			//Query all subkeys under "SYSTEM\\CurrentControlSet\Services" and save them to keyServ  
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services"), 0, KEY_READ | KEY_WOW64_32KEY, &keyServ);
			if(ERROR_SUCCESS != lResult)
				return nullptr;

			//Query all subkeys under "SYSTEM\\CurrentControlSet\\Enum" and save them to keyEnum  
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum"), 0, KEY_READ | KEY_WOW64_32KEY, &keyEnum);
			if(ERROR_SUCCESS != lResult)
				return nullptr;

			DWORD size = 0;
			for(int i = 0, count = 0;; ++i)
			{
				size = 512;
				TCHAR name[512] = { 0 };//Save the field names of subitems under keyServ  

				//Enumerate each subitem field under keyServ one by one and save it in name  
				lResult = RegEnumKeyEx(keyServ, i, name, &size, NULL, NULL, NULL, NULL);

				//Subitems to be read do not exist, that is, keyServ's subitems jump out of the loop when they are all traversed  
				if(lResult == ERROR_NO_MORE_ITEMS)
					break;

				// Get only NVIDIA drivers for now
				if(strcmp(name, "nvlddmkm") == 0)
				{

					//Open the keyServ subentry field and save the value of the field identified by name to key  
					lResult = RegOpenKeyEx(keyServ, name, 0, KEY_READ | KEY_WOW64_32KEY, &key);
					if(lResult != ERROR_SUCCESS)
					{
						RegCloseKey(keyServ);
						return nullptr;
					}

					size = 512;
					//The field under query key is the subkey field name of Group saved to name  
					lResult = RegQueryValueEx(key, TEXT("Group"), 0, 0, (LPBYTE)name, &size);
					if(lResult == ERROR_FILE_NOT_FOUND)
					{
						//The? Key does not exist  
						RegCloseKey(key);
						continue;
					}

					//If the name queried is not Video, the key is not a graphics card driver.  
					if(_tcscmp(TEXT("Video"), name) != 0)
					{
						RegCloseKey(key);
						continue;     //Return to the for loop  
					}

					//If the program continues to execute, the information about the graphics card has been found, so after the following code is executed, break the first for loop, and the function returns.  
					lResult = RegOpenKeyEx(key, TEXT("Enum"), 0, KEY_READ | KEY_WOW64_32KEY, &key2);
					RegCloseKey(key);
					key = key2;
					size = sizeof(count);
					lResult = RegQueryValueEx(key, TEXT("Count"), 0, 0, (LPBYTE)&count, &size);//Query Count field (number of graphics cards)  

					for(int j = 0; j < count; ++j)
					{
						TCHAR sz[512] = { 0 };
						TCHAR name[64] = { 0 };
						wsprintf(name, TEXT("%d"), j);
						size = sizeof(sz);
						lResult = RegQueryValueEx(key, name, 0, 0, (LPBYTE)sz, &size);

						lResult = RegOpenKeyEx(keyEnum, sz, 0, KEY_READ | KEY_WOW64_32KEY, &key2);
						if(ERROR_SUCCESS)
						{
							RegCloseKey(keyEnum);
							return nullptr;
						}

						for(int m = 0; m < Device::MembersCount(); m++)
						{
							if(Device::GetMemberType(m) == typeid(unsigned long))
							{
								DWORD dw;
								DWORD bsize = sizeof(dw);
								if(RegQueryValueEx(key2, TEXT(Device::GetMemberName(m)), 0, 0, reinterpret_cast<LPBYTE>(&dw), &bsize) == ERROR_SUCCESS)
								{
									d.SetMemberValue<unsigned long>(m, dw);
								}
							}
							if(Device::GetMemberType(m) == typeid(std::string))
							{
								TCHAR sz2[512] = { 0 };
								DWORD size2 = 0;
								size2 = sizeof(sz2);
								if(RegQueryValueEx(key2, TEXT(Device::GetMemberName(m)), 0, 0, (LPBYTE)sz2, &size2) == ERROR_SUCCESS)
								{
									d.SetMemberValue<std::string>(m, ((LPCTSTR)sz2));
								}
							}
						}
					}
				}
			}

			if(StringContainsInsensitive(d.Mfg, "NVIDIA"))
			{
				p = Factory::Create<GPU>(d, Manufacturer::NVIDIA);
			}
			else if(StringContainsInsensitive(d.Mfg, "AMD"))
			{
				p = Factory::Create<GPU>(d, Manufacturer::AMD);
			}
		}

		RegCloseKey(key2);
		key2 = nullptr;
		RegCloseKey(key);
		key = nullptr;

		return p;
	};

protected:

	Peripheral(Device device)
		:
		m_Device(device),
		m_Workload(0)
	{ }

	Device m_Device;
	long m_Workload;
};