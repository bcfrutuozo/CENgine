#include "ShadowCameraCbuf.h"
#include "Camera.h"

namespace Bind
{
	ShadowCameraCbuf::ShadowCameraCbuf(Graphics& graphics, UINT slot)
		:
		pVertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(graphics, slot) }
	{ }

	void ShadowCameraCbuf::Bind(Graphics& graphics) NOXND
	{
		pVertexConstantBuffer->Bind(graphics);
	}

	void ShadowCameraCbuf::Update(Graphics& graphics)
	{
		const Transform t{
			DirectX::XMMatrixTranspose(
				pCamera->GetMatrix() * pCamera->GetProjection()
			)
		};
		pVertexConstantBuffer->Update(graphics, t);
	}

	void ShadowCameraCbuf::SetCamera(const Camera* p) noexcept
	{
		pCamera = p;
	}
}
