#pragma once

#include <vector>
#include <string>

std::vector<std::string> TokenizedQuoted(const std::string& input);
std::wstring ToWide(const std::string& s);
std::string ToNarrow(const std::wstring& s);