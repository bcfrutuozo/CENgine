#pragma once

#include "RenderGraph.h"

class Graphics;

namespace RGP
{
	class ScaleOutlineRenderGraph : public RenderGraph
	{
	public:

		ScaleOutlineRenderGraph(Graphics& graphics);
	};
}