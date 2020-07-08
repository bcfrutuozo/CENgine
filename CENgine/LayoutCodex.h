#pragma once

#include "DynamicConstant.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace DRR
{
	class LayoutCodex
	{
	public:

		static CompleteLayout Resolve(IncompleteLayout&& layout) NOXND;
	private:

		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> map;
	};
}