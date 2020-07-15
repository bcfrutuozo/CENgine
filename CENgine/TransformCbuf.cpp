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

	void TransformCbuf::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(UpdateBindImpl(graphics, GetTransforms(graphics)));
	}

	void TransformCbuf::InitializeParentReference( const Drawable& parent ) noexcept
	{
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> TransformCbuf::Clone() const noexcept
	{
		return std::make_unique<TransformCbuf>( *this );
	}

	void TransformCbuf::UpdateBindImpl(Graphics& graphics, const Transforms& tf) NOXND
	{
		assert( pParent != nullptr );

		pVertexConstantBuffer->Update(graphics, tf);
		pVertexConstantBuffer->Bind(graphics);
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& graphics) NOXND
	{
		assert( pParent != nullptr );

		const auto model = pParent->GetTransformXM();
		const auto modelView = model * graphics.GetCamera();
		return {
			DirectX::XMMatrixTranspose(model),
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * graphics.GetProjection())
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVertexConstantBuffer;
}