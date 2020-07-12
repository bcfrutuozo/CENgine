#include "TransformCbuf.h"

namespace Bind 
{
	TransformCbuf::TransformCbuf(Graphics& graphics, UINT slot)
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

	void TransformCbuf::InitializeParentReference( const Drawable& parent ) noexcept
	{
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> TransformCbuf::Clone() const noexcept
	{
		return std::make_unique<TransformCbuf>( *this );
	}

	void TransformCbuf::UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept
	{
		assert( pParent != nullptr );
		pVertexConstantBuffer->Update(graphics, tf);
		pVertexConstantBuffer->Bind(graphics);
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& graphics) noexcept
	{
		assert( pParent != nullptr );
		const auto modelView = pParent->GetTransformXM() * graphics.GetCamera();
		return {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * graphics.GetProjection())
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVertexConstantBuffer;
}