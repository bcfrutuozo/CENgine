#pragma once

#include "Conditional_noexcept.h"

class Graphics;

namespace Bind
{
	class BufferResource
	{
	public:

		virtual ~BufferResource() = default;
		virtual void BindAsBuffer(Graphics&) NOXND = 0;
		virtual void BindAsBuffer(Graphics&, BufferResource*) NOXND = 0;
		virtual void Clear(Graphics&) NOXND = 0;
	};
}