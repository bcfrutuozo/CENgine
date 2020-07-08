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
