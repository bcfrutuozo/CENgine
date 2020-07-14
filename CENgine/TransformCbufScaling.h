#pragma once

#include "TransformCbuf.h"
#include "DynamicConstant.h"

namespace Bind
{
	class TransformCbufScaling : public TransformCbuf
	{
	public:

		TransformCbufScaling(Graphics& graphics, float scale);
		void Accept(TechniqueProbe& probe) override;
		void Bind(Graphics& graphics) NOXND override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
	private:

		static DRR::IncompleteLayout MakeLayout();

		DRR::Buffer buffer;
	};
}