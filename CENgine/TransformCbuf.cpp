#include "TransformCbuf.h"

namespace Bind {

	TransformCbuf::TransformCbuf(Graphics& graphics, const Drawable& parent, UINT slot)
		:
		parent(parent)
	{
		if (!pVertexConstantBuffer)
		{
			pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(graphics, slot);
		}
	}

	void TransformCbuf::Bind(Graphics& graphics) noexcept
	{
		UpdateBindImpl(graphics, GetTransforms(graphics));
	}

	void TransformCbuf::UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept
	{
		pVertexConstantBuffer->Update(graphics, tf);
		pVertexConstantBuffer->Bind(graphics);
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& graphics) noexcept
	{
		const auto modelView = parent.GetTransformXM() * graphics.GetCamera();
		return {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * graphics.GetProjection())
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVertexConstantBuffer;
}