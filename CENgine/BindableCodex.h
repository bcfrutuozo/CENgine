#pragma once

#include "Bindable.h"
#include <memory>
#include <unordered_map>
#include <type_traits>

namespace Bind	{

	class Codex
	{
	public:

		template<class T, typename... Params>
		static std::shared_ptr<T> Resolve(Graphics& graphics, Params&&... p) NOXND
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(graphics, std::forward<Params>(p)...);
		}

	private:

		template<class T, typename... Params>
		std::shared_ptr<T> Resolve_(Graphics& graphics, Params&&... p) NOXND
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto i = binds.find(key);
			
			if(i == binds.end())
			{
				auto bind = std::make_shared<T>(graphics, std::forward<Params>(p)...);
				binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
		}

		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}

		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
	};
}