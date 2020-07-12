#pragma once

#include "Graphics.h"
#include "Conditional_noexcept.h"
#include "GraphicsResource.h"

#include <memory>
#include <string>

class Drawable;
class TechniqueProbe;

namespace Bind 
{	
	class Bindable : public GraphicsResource
	{
	public:

		virtual void Bind(Graphics& graphics) noexcept = 0;
		
		virtual void InitializeParentReference(const Drawable&) noexcept
		{ }

		virtual void Accept(TechniqueProbe&)
		{ }

		virtual std::string GetUID() const noexcept
		{
			assert(false);
			return "";
		}
		
		virtual ~Bindable() = default;
	};

	class CloningBindable : public Bindable
	{
	public:

		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}