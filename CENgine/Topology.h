#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	class Topology : public Bindable
	{
	public:
		
		Topology(Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(Graphics& graphics) NOXND override;
		static std::shared_ptr<Topology> Resolve(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type);
		std::string GetUID() const noexcept override;
	protected:
		
		D3D11_PRIMITIVE_TOPOLOGY type;
	};
}