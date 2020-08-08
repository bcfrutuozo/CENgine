#include "Utilities.h"

#include <sstream>
#include <iomanip>

std::vector<std::string> TokenizedQuoted(const std::string& input)
{
	std::istringstream stream;
	stream.str(input);
	std::vector<std::string> tokens;
	std::string token;

	while(stream >> std::quoted(token))
	{
		tokens.push_back(std::move(token));
	}

	return tokens;
}

std::wstring ToWide(const std::string& s)
{
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, s.c_str(), _TRUNCATE);
	return wide;
}

std::string ToNarrow(const std::wstring& s)
{
	char narrow[512] = "\0";
	wcstombs_s(nullptr, narrow, s.c_str(), _TRUNCATE);
	return narrow;
}

std::string GetLastSubstring(const std::string& s, const std::string& separator)
{
	const auto& size = s.find_last_of(separator);
	return s.substr(size + 1);
}

std::wstring GetLastWSubstring(const std::wstring& s, const std::wstring& separator)
{
	const auto& size = s.find_last_of(separator);
	return s.substr(size + 1);
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim)
{
	std::vector<std::string> strings;
	SplitStringIter(s, delim, std::back_inserter(strings));
	return strings;
}

bool StringContains(const std::string_view haystack, const std::string_view needle)
{
	return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end()) != haystack.end();
}

bool StringContainsInsensitive(const std::wstring& haystack, const std::wstring& needle)
{
	auto it = std::search(
		haystack.begin(), haystack.end(),
		needle.begin(), needle.end(),
		[](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
	);

	return (it != haystack.end());
}

GUID StringToGuid(std::string str)
{
	GUID guid;
	sscanf_s(str.c_str(),
		"{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
		&guid.Data1, &guid.Data2, &guid.Data3,
		&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
		&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);

	return guid;
}


std::string GuidToString(GUID guid)
{
	char guid_cstr[39];
	snprintf(guid_cstr, sizeof(guid_cstr),
		"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return std::string(guid_cstr);
}