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
		const auto pos = pCamera->GetPosition();
		const Transform t{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
			)
		};
		pVertexConstantBuffer->Update(graphics, t);
	}

	void ShadowCameraCbuf::SetCamera(const Camera* p) noexcept
	{
		pCamera = p;
	}
}
