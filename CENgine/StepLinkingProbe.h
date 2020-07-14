#pragma once

#include "TechniqueProbe.h"

namespace RGP
{
	class RenderGraph;
}

class StepLinkingProbe : public TechniqueProbe
{
protected:

	void OnSetStep() override;
private:

	RGP::RenderGraph& renderGraph;
};

