#include "Topology.h"

namespace Bind {
	
	Topology::Topology(Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type)
		:
		type(type)
	{}

	void Topology::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->IASetPrimitiveTopology(type);
	}
}