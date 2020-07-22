#include "SkyboxTransformCbuf.h"

namespace Bind
{
	SkyboxTransformCbuf::SkyboxTransformCbuf(Graphics& graphics, UINT slot)
		:
		pVertexConstantBuffer(std::make_unique<VertexConstantBuffer<Transforms>>(graphics, slot))
	{ }

	void SkyboxTransformCbuf::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(UpdateBindImpl(graphics, GetTransforms(graphics)));
	}

	void SkyboxTransformCbuf::UpdateBindImpl(Graphics& graphics, const Transforms& tf) NOXND
	{
		pVertexConstantBuffer->Update(graphics, tf);
		pVertexConstantBuffer->Bind(graphics);
	}

	SkyboxTransformCbuf::Transforms SkyboxTransformCbuf::GetTransforms(Graphics& graphics) NOXND
	{
		return {
			DirectX::XMMatrixTranspose(graphics.GetCamera() * graphics.GetProjection
			()) 
		};
	}
}