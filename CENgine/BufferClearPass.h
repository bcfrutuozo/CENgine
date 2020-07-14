#pragma once

#include "Pass.h"

namespace Bind
{
	class BufferResource;
}

namespace RGP
{
	class BufferClearPass : public Pass
	{
	public:

		BufferClearPass(std::string name);
		void Execute(Graphics& graphics) const NOXND override;
	private:

		std::shared_ptr<Bind::BufferResource> buffer;
	};
}