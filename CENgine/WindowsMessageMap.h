#pragma once

#include "CENgineWin.h"

#include <unordered_map>

class WindowsMessageMap
{
public:
	WindowsMessageMap() noexcept;
	std::string operator() (DWORD message, LPARAM lParam, WPARAM wParam) const noexcept;
private:
	std::unordered_map<DWORD, std::string> map;
};