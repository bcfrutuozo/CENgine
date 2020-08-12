#pragma once

#include "BindingPass.h"

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
	class VertexShader;
	class InputLayout;
}

namespace RGP
{
	class FullscreenPass : public BindingPass
	{
	public:

		FullscreenPass(std::string name, Graphics& graphics) NOXND;
		void Execute(Graphics& graphics) const NOXND override;
	};
}