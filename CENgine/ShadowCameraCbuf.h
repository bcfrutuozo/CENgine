#pragma once

#include "Bindable.h"
#include "ConstantBuffer.h"

class Camera;

namespace Bind
{
	class ShadowCameraCbuf : public Bindable
	{
	protected:

		struct Transform
		{
			DirectX::XMMATRIX ViewProjection;
		};

	public:

		ShadowCameraCbuf(Graphics& graphics, UINT slot = 1u);
		void Bind(Graphics& graphics) NOXND override;
		void Update(Graphics& graphics);
		void SetCamera(const Camera* camera) noexcept;
	private:

		std::unique_ptr<VertexConstantBuffer<Transform>> pVertexConstantBuffer;
		const Camera* pCamera = nullptr;
	};
}
