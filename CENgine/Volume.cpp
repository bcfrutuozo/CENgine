#include "Volume.h"
#include "PeripheralThrowMacros.h"

#pragma warning(push, 0)
#include "imgui/imgui.h"
#pragma warning(pop)

#include <iomanip>

Volume::Volume(Device p_Device)
	:
	m_IsValidVolume(false),
	Peripheral(p_Device)
{
 	auto s = ToNarrow(p_Device.Enumerator);
	auto ref = s.find("#") + 1;

	if (ref != 0)
	{
		auto n = FindVolumeByDriverEnumerator(s, Drive::Type::DISK);

		if (!n.empty())
		{
			m_Name = n;
			m_IsValidVolume = true;
		}
	}
	else
	{
		s = s.substr(s.find("Volume") + 7);
		m_Name = StringFormat("\\\\?\\Volume%s\\", s.c_str());
		m_IsValidVolume = true;
	}
}

Volume::~Volume()
{
}

bool Volume::Initialize()
{
	// 32 character for readable volume name plus '\0'
	char name[64];
	char letters[32];

	char fs[8];
	DWORD ser = 0, mcl = 0, fsf = 0, ret = 0;

	if (m_IsValidVolume)
	{
		if (!GetVolumeInformation(m_Name.c_str(), name, 64, &ser, &mcl, &fsf, fs, 8))
		{
			throw PRPH_LAST_EXCEPT();
		}

		if (!GetVolumePathNamesForVolumeName(m_Name.c_str(), letters, 32, &ret))
		{
			throw PRPH_LAST_EXCEPT();
		}

		// Check if c string is empty;
		if (*name != 0)
		{
			m_ReadableName = std::string(name);
		}

		m_FileSystem = std::string(fs);
	}

	return true;
}

void Volume::GetWorkload()
{
	if (m_IsValidVolume)
	{
		bool fResult;

		ULARGE_INTEGER fbc = { 0 };
		ULARGE_INTEGER tb = { 0 };
		ULARGE_INTEGER fb = { 0 };

		// TODO: CHECK FOR RAID 0 TO GET THE REAL AVAILABLE SPACE OF THE DISK.

		fResult = GetDiskFreeSpaceEx(m_Name.c_str(),
			(PULARGE_INTEGER)&fbc,
			(PULARGE_INTEGER)&tb,
			(PULARGE_INTEGER)&fb);

		m_FreeBytesToCaller = fbc.QuadPart;
		m_TotalSize = tb.QuadPart;
		m_FreeSize = fb.QuadPart;

		if (fResult)
		{
			m_TotalSize = m_TotalSize / 1073741824;
			m_FreeSize = m_FreeSize / 1073741824;
		}
	}
}

void Volume::ShowWidget()
{
	if (m_IsValidVolume && !m_ReadableName.empty())
	{
		if (ImGui::TreeNode(m_ReadableName.c_str()))
		{
			ImGui::Text("Volume Name: %s", m_ReadableName.c_str());
			ImGui::Text("Free space: %I64u GBs", m_FreeSize);
			ImGui::Text("Total size: %d GBs", m_TotalSize);
			ImGui::Text("File system: %s", m_FileSystem.c_str());

			ImGui::TreePop();
		}
	}
}

const std::string Volume::FindVolumeByDriverEnumerator(const std::string& p_RegistryEnumerator, Drive::Type p_Type)
{
	RegKey root = { HKEY_CURRENT_USER, MountsPoint, KEY_READ | KEY_WOW64_32KEY };

	std::string ret;
	auto entries = root.EnumSubKeys();

	for (const auto& k : entries)
	{
		RegKey key;

		std::string enm;
		size_t beg;
		switch (p_Type)
		{
		case Drive::Type::DISK:
			enm = p_RegistryEnumerator.substr(p_RegistryEnumerator.find_last_of("\\") + 1);
			break;

		case Drive::Type::CDROM:
			enm = p_RegistryEnumerator;
			beg = enm.find("CdRom") + 5;
			enm = enm.substr(beg + 1);
			enm.replace(enm.find('\\'), 1, "#");
			break;

		default:
			throw PRPH_UNSUPPORTED_TYPE_EXCEPT(static_cast<long>(p_Type));
		}

		if (key.TryOpen(root.Get(), k, KEY_READ | KEY_WOW64_32KEY))
		{
			auto p = key.GetBinaryValue(L"Data");

			// Need to get a decent structure to parse the registry binary information. 
			std::stringstream wss;

			// Reading registry mount points data value
			for (unsigned short i = 0x52; i < p.size() - 0x52; i++)
			{
				if (p[i] != '\0')
					wss << p[i];
			}

			// Close key only if it opened and after copying the buffer
			key.Close();

			std::string data = wss.str();
			wss << std::endl;

			auto indexOccurence = std::count(data.begin(), data.end(), '#');
			size_t begin;
			size_t end;
			std::string index;
			if (p_Type == Drive::Type::DISK)
			{
				if (indexOccurence < 4)
				{
					// If contains less than 4 # in the string, means it's a dynamic volume and its logical name is already the default found in the registry enum drivers.
					continue;
				}

				begin = data.find("#Volume#") + 8;
				end = data.find_last_of('#') - begin;
				index = data.substr(begin, end);
			}
			else if (p_Type == Drive::Type::CDROM)
			{
				if (indexOccurence > 3)
				{
					continue;
				}

				begin = data.find("#CdRom") + 7;
				end = data.find_last_of('#') - begin;
				index = data.substr(begin, end);
			}
			else
			{
				throw PRPH_UNSUPPORTED_TYPE_EXCEPT(static_cast<long>(p_Type));
			}

			if (index == enm)
			{
				// Use the last calculation as offset
				begin = data.find("\\", end);
				end = begin + 55;

				ret = data.substr(begin, 49);	// For volume name, we must keep the backslash at the end, so that's why we're adding 1 at the 48 size string.
				break;
			}
		}
	}

	root.Close();

	return ret;
}
