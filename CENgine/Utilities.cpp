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
    char narrow[512];
    wcstombs_s(nullptr, narrow, s.c_str(), _TRUNCATE);
    return narrow;
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim)
{
	std::vector<std::string> strings;
	SplitStringIter(s, delim, std::back_inserter(strings));
	return strings;
}

bool StringContains(std::string_view haystack, std::string_view needle)
{
    return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end()) != haystack.end();
}