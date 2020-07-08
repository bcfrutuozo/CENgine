#include "LayoutCodex.h"

namespace DRR
{
	CompleteLayout LayoutCodex::Resolve(IncompleteLayout&& layout) NOXND
	{
		auto sig = layout.GetSignature();
		auto& map = Get_().map;
		const auto i = map.find(sig);

		// Identical layout already exists
		if(i != map.end())
		{
			// Input layout is expected to be cleared after Resolve
			// so just throw the layout tree away
			layout.ClearRoot();
			return { i->second };
		}

		// Otherwise add layout root element to map
		auto result = map.insert({ std::move(sig), layout.DeliverRoot() });

		// Return layout with additional reference to root
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get_() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}