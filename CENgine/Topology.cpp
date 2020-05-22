#include "Topology.h"
#include "BindableCodex.h"

namespace Bind {
	
	Topology::Topology(Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type)
		:
		type(type)
	{}

	void Topology::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->IASetPrimitiveTopology(type);
	}

	std::shared_ptr<Topology> Topology::Resolve(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type)
	{
		return Codex::Resolve<Topology>(graphics, type);
	}

	std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type)
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + std::to_string(type);
	}

	std::string Topology::GetUID() const noexcept
	{
		return GenerateUID(type);
	}
}
