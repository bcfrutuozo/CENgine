#pragma once

#include "CENgineWin.h"
#include "CENgineWRL.h"

#include <vector>
#include <dxgidebug.h>
#include <string>

class DXGIInfoManager
{
public:
	DXGIInfoManager();
	~DXGIInfoManager() = default;
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDXGIInfoQueue;
};

