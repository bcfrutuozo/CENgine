#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <guiddef.h>
#include <stdexcept>
#include <memory>

std::vector<std::string> TokenizedQuoted(const std::string& input);
std::wstring ToWide(const std::string& s);
std::string ToNarrow(const std::wstring& s);
std::string GetLastSubstring(const std::string& s, const std::string& separator);
std::wstring GetLastWSubstring(const std::wstring& s, const std::wstring& separator);

template<class Iter>
void SplitStringIter(const std::string& s, const std::string& delim, Iter out)
{
	if (delim.empty())
	{
		*out++ = s;
	}
	else
	{
		size_t a = 0, b = s.find(delim);
		for (; b != std::string::npos; a = b + delim.length(), b = s.find(delim, a))
		{
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim);

bool StringContains(const std::string_view haystack, const std::string_view needle);

bool StringContainsInsensitive(const std::wstring& haystack, const std::wstring& needle);

GUID StringToGuid(std::string str);

std::string GuidToString(GUID guid);