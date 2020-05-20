#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& graphics, const Drawable& parent, UINT slot)
	:
	parent(parent)
{
	if(!pVertexConstantBuffer)
	{
		pVertexConstantBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(graphics, slot);
	}
}

void TransformCbuf::Bind(Graphics& graphics) noexcept
{
	const auto& modelView = parent.GetTransformXM() * graphics.GetCamera();
	const Transforms tf = {
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(modelView * graphics.GetProjection())
	};
	
	pVertexConstantBuffer->Update(graphics, tf);
	pVertexConstantBuffer->Bind(graphics);
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVertexConstantBuffer;